//
// Created by cassiano on 09/09/2020.
//


#include "requadrangulator.h"

#include "opsketch.h"
#include "opquadmesh.h"
#include "optrimesh.h"
#include "patchquadrangulator.h"

#include "utils/openmesh.h"
#include "utils/debug.h"

#include <queue>
#include <algorithm>
#include <iterator>

#include <Eigen/Sparse>

skbar::Requadrangulator::Requadrangulator(skbar::EditableMesh *_mesh) : editableMesh(_mesh) {

}

void skbar::Requadrangulator::RequadrangulateAll() {

    // TODO Check if sketch is closed

    auto affectedPatches = FindAffectedPatches();

    for (const auto &entry : affectedPatches) {
        RequadrangulatePatch(entry.first, entry.second);
    }
}

std::map<int, std::vector<std::tuple<int, int>>> skbar::Requadrangulator::FindAffectedPatches() const {
    std::map<int, std::vector<std::tuple<int, int>>> result;

    auto &sketch = dynamic_cast<OPSketch &>(editableMesh->GetSketch());

    auto &quadmesh = dynamic_cast<OPQuadMesh &>(editableMesh->GetQuad()).Get();
    auto &trimesh = dynamic_cast<OPTriMesh &>(editableMesh->GetTri()).Get();

    auto firstPatch = -1;
    auto currentPatch = -1;

//    std::vector<Vec2f> points;
    std::vector<std::vector<Vec2f>> sides;
    int firstSketchVertex = -1;
    int lastSketchVertex = -1;

    // TODO Check to all patches
    {
        for (unsigned int i = 0; i < sketch.Size(); i++) {

            const auto &sv = sketch.Data()[i];

            if (sv.Type() == SketchVertex::EType::FACE) {

                const auto faceId = trimesh.data(OpenMesh::FaceHandle(sv.Pointer())).triFaceData.quadFaceId;
                const auto face = OpenMesh::FaceHandle(faceId);

                currentPatch = quadmesh.data(face).quadFaceData.patchId;

                if (firstPatch == -1) {
                    firstPatch = currentPatch;
                }

            } else if (sv.Type() == SketchVertex::EType::EDGE) {

//            const auto vertex = OpenMesh::SmartVertexHandle(sv.Pointer(), quadmesh);
                const auto &triEdgeData = trimesh.data(OpenMesh::EdgeHandle(sv.Pointer())).triEdgeData;

                if (triEdgeData.IsQuadEdge()) {
                    const auto edge = OpenMesh::SmartEdgeHandle(triEdgeData.quadEdgeId, &quadmesh);

                    if (quadmesh.data(edge).quadEdgeData.patchEdge) {
                        const auto f0 = edge.h0().face();
                        const auto f1 = edge.h1().face();

                        const auto face = (quadmesh.data(f0).quadFaceData.patchId == currentPatch) ? f1 : f0;

                        currentPatch = quadmesh.data(face).quadFaceData.patchId;

                        sides.emplace_back();
                        sides.back().push_back(sv.ParametricPositionsByPatch().at(currentPatch));

                        firstSketchVertex = static_cast<int>(i);

                        break;
                    }
                }
            }
        }

        // Pass through all sketch vertices
        for (unsigned int i = static_cast<unsigned int>(firstSketchVertex) + 1; i < sketch.Size(); i++) {

            const auto &sv = sketch.Data()[i];

//        try {
//            sides.back().push_back(sv.ParametricPositionsByPatch().at(currentPatch));
//        } catch (const std::out_of_range &e) {
//            Log("Out of range! %d", currentPatch);
//        }

            sides.back().push_back(sv.ParametricPositionsByPatch().at(currentPatch));

            if (sv.Type() == SketchVertex::EType::EDGE) {

                const auto triEdgeData = trimesh.data(OpenMesh::EdgeHandle(sv.Pointer())).triEdgeData;

                if (triEdgeData.IsQuadEdge()) {
                    const auto edge = OpenMesh::SmartEdgeHandle(triEdgeData.quadEdgeId, &quadmesh);

                    if (quadmesh.data(edge).quadEdgeData.patchEdge) {
                        lastSketchVertex = static_cast<int>(i);

                        break;
                    }
                }

//            const auto edge = OpenMesh::SmartEdgeHandle(sv.Pointer(), &quadmesh);
//
//            if (quadmesh.data(edge).quadEdgeData.patchEdge) {
//                lastEdgeIndex = sv.Pointer();
//
//                break;
//            }
            }
        }

        auto otherSides = FindSidesOfPatch(sketch.Data().at(firstSketchVertex), sketch.Data().at(lastSketchVertex));

        result.emplace(currentPatch,
                       std::vector(1, std::make_tuple(firstSketchVertex, lastSketchVertex)));
    }


    return result;
}

void skbar::Requadrangulator::RequadrangulatePatch(int patch, const std::vector<std::tuple<int, int>> &inOuts) {

    // If doesn't have any an in-out, then the sketch is contained inside one single patch, creating a hole inside of it
    const auto hasHole = inOuts.empty();

    if (!hasHole) {
        RequadrangulatePatchWithoutHole(patch, inOuts);

    } else {
        // TODO Implement this
    }

//    auto otherSides = FindSidesOfPatch(sketch.Data().at(firstSketchVertex), sketch.Data().at(lastSketchVertex));
//
//    sides.insert(sides.end(), otherSides.begin(), otherSides.end());

//    OPQuadMesh newPatch;
//
//    Eigen::VectorXi patchSides;
//
//    for (std::size_t i = 0; i < sides.size(); i++) {
//        patchSides(i) = static_cast<int>(sides.at(i).size());
//    }
//
//    auto param = PatchQuadrangulator::ComputeTopology(patchSides, newPatch);
}

void
skbar::Requadrangulator::RequadrangulatePatchWithoutHole(int patch, const std::vector<std::tuple<int, int>> &inOuts) {

    auto &sketch = dynamic_cast<OPSketch &>(editableMesh->GetSketch());

    auto &quadmesh = dynamic_cast<OPQuadMesh &>(editableMesh->GetQuad()).Get();
    auto &trimesh = dynamic_cast<OPTriMesh &>(editableMesh->GetTri()).Get();

    std::vector<int> edgesBySide;
    std::vector<Vec2f> borderVertexPositions;
    std::vector<int> borderVertices;

    std::map<OpenMesh::SmartHalfedgeHandle, std::tuple<unsigned int, bool>> halfEdgesToCheck;

    for (unsigned int i = 0; i < inOuts.size(); i++) {
        const auto &[in, out] = inOuts[i];

        const auto &inSketchVertex = sketch.Data().at(in);
        const auto &outSketchVertex = sketch.Data().at(out);

        const auto inEdgeId = trimesh.data(OpenMesh::EdgeHandle(inSketchVertex.Pointer())).triEdgeData.quadEdgeId;
        const auto outEdgeId = trimesh.data(OpenMesh::EdgeHandle(outSketchVertex.Pointer())).triEdgeData.quadEdgeId;

        const auto inEdge = OpenMesh::SmartEdgeHandle(inEdgeId, &quadmesh);
        const auto outEdge = OpenMesh::SmartEdgeHandle(outEdgeId, &quadmesh);

        const auto inHE = (quadmesh.data(inEdge.h0().face()).quadFaceData.patchId == patch) ?
                          inEdge.h0() : inEdge.h1();
        const auto outHE = (quadmesh.data(outEdge.h0().face()).quadFaceData.patchId == patch) ?
                           outEdge.h0() : outEdge.h1();

        halfEdgesToCheck[inHE] = std::make_tuple(i, false);
        halfEdgesToCheck[outHE] = std::make_tuple(i, false);
    }

    for (const auto &entry : halfEdgesToCheck) {
        const auto &firstHE = entry.first;
        const auto[inOutId, checked] = entry.second;

        if (!checked) {
            const auto[inId, outId] = inOuts[inOutId];

            // Used to add sketch vertices in reverse order if is true
            const auto startingFromOut = (firstHE.edge().idx() == sketch.Data().at(outId).Pointer());

            const auto &lastEdge = startingFromOut
                                   ? OpenMesh::SmartEdgeHandle(sketch.Data().at(inId).Pointer(), &quadmesh)
                                   : OpenMesh::SmartEdgeHandle(sketch.Data().at(outId).Pointer(), &quadmesh);

            auto currentHE = firstHE;

            // TODO Check if in == out
            auto reachedLastEdge = false;

//            // Add the first sketch vertex position
            {
                const auto &sketchVertexToAdd = sketch.Data().at(startingFromOut ? outId : inId);

                borderVertexPositions.push_back(sketchVertexToAdd.ParametricPositionsByPatch().at(patch));

                const auto newVertexId = AddSketchVertexToQuadMesh(sketchVertexToAdd);

                borderVertices.push_back(newVertexId);
            }

            while (!reachedLastEdge) {

                auto reachedCorner = quadmesh.data(currentHE.to()).quadVertexData.isCorner;

                auto countEdges = 1;

                while (!reachedCorner) {

                    if (currentHE.edge() == lastEdge) {
                        reachedLastEdge = true;
                        break;
                    }

                    countEdges++;

                    const auto &parametricPosition = quadmesh.data(
                            currentHE.to()).quadVertexData.patchParametrizations[patch];

                    borderVertexPositions.push_back(parametricPosition);
                    borderVertices.push_back(currentHE.to().idx());

                    currentHE = currentHE.next().opp().next();

                    reachedCorner = quadmesh.data(currentHE.to()).quadVertexData.isCorner;
                }

                if (!reachedLastEdge) {
                    const auto &parametricPosition = quadmesh.data(
                            currentHE.to()).quadVertexData.patchParametrizations[patch];

                    // Don't forget the last vertex
                    borderVertexPositions.push_back(parametricPosition);
                    borderVertices.push_back(currentHE.to().idx());

                    currentHE = currentHE.next();
                }

                edgesBySide.push_back(countEdges);
            }

            auto countEdges = 0;
            std::vector<skbar::Vec2f> sketchPositions;

            auto next = [](auto i, auto size, bool reverse) -> auto {
                return reverse ? ((i + size - 1) % size) : ((i + 1) % size);
            };

            {
                const auto first = startingFromOut ? inId : outId;
                const auto last = startingFromOut ? outId : inId;

                for (std::size_t i = first; i != static_cast<std::size_t>(last);
                     i = next(i, sketch.Size(), !startingFromOut)) {

                    const auto &sketchVertex = sketch.Data().at(i);

                    // TODO Check for vertices on a face
                    if (sketchVertex.Type() == skbar::SketchVertex::EType::EDGE) {
                        const auto triVertex = OpenMesh::EdgeHandle(sketchVertex.Pointer());

                        const auto isQuadEdge = trimesh.data(triVertex).triEdgeData.IsQuadEdge();

                        if (isQuadEdge) {
                            countEdges++;

                            sketchPositions.push_back(sketchVertex.ParametricPositionsByPatch().at(patch));

                            const auto newVertexId = AddSketchVertexToQuadMesh(sketchVertex);

                            borderVertices.push_back(newVertexId);
                        }


                    }
                }
            }

            edgesBySide.push_back(countEdges);

            borderVertexPositions.insert(borderVertexPositions.end(), sketchPositions.begin(),
                                         sketchPositions.end());

        }

        std::get<1>(halfEdgesToCheck.at(firstHE)) = true;

        // TODO Find a better place to do this
        {
            Eigen::VectorXi sides(edgesBySide.size());
            std::vector<OpenMesh::Vec3d> parametricPositions;

            for (std::size_t i = 0; i < edgesBySide.size(); i++) {
                sides(i) = edgesBySide[i];
            }

            const auto borderSize = borderVertexPositions.size();

            for (std::size_t i = 0; i < borderSize; i++) {
                const auto position = borderVertexPositions[i];

                parametricPositions.emplace_back(position[0], position[1], 0);
            }

            const auto newPolygonCCW = PatchQuadrangulator::Quadrangulate(sides, parametricPositions, false);
            const auto newPolygonCW = PatchQuadrangulator::Quadrangulate(sides, parametricPositions, true);

            newPolygonCCW.Save("./test_ccw.obj");
            newPolygonCW.Save("./test_cw.obj");

            const auto newPatchToQuadMesh = MapNewPatchToQuadMesh(editableMesh->GetQuad(), newPolygonCCW,
                                                                  borderVertices);

            DeletePatchFaces(patch);
            CreateNewFacesOnQuadMesh(editableMesh->GetQuad(), patch, newPolygonCCW, newPatchToQuadMesh);

            quadmesh.request_face_normals();

//            // TODO Save a copy before clean deleted vertices
//            quadmesh.garbage_collection();
        }

        // TODO Check other HEs
        break;
    }
}

std::vector<std::vector<skbar::Vec2f>>
skbar::Requadrangulator::FindSidesOfPatch(const skbar::SketchVertex &firstSV, const skbar::SketchVertex &lastSV) const {
    std::vector<std::vector<Vec2f>> sides;

    const auto &quadmesh = dynamic_cast<const OPQuadMesh &>(editableMesh->GetQuad()).Get();
    const auto &trimesh = dynamic_cast<const OPTriMesh &>(editableMesh->GetTri()).Get();

    const auto patches = FindCommonPatches(firstSV, lastSV);

    // TODO Remove this line
    assert(!patches.empty() && "Must have at least one common patch");

    // TODO Use all patches
    const auto patch = patches.front();

    const OpenMesh::EdgeHandle firstTriEdge(firstSV.Pointer());
    const OpenMesh::EdgeHandle lastTriEdge(lastSV.Pointer());

    const OpenMesh::SmartEdgeHandle firstEdge(trimesh.data(firstTriEdge).triEdgeData.quadEdgeId, &quadmesh);
    const OpenMesh::SmartEdgeHandle lastEdge(trimesh.data(lastTriEdge).triEdgeData.quadEdgeId, &quadmesh);

    if (!firstEdge.is_valid() || !lastEdge.is_valid()) {
        return sides;
    }

    // TODO Also check the other HE
    auto currentHE = (quadmesh.data(firstEdge.h0().face()).quadFaceData.patchId == patch)
                     ? firstEdge.h0() : firstEdge.h1();

//    currentHE = currentHE.next().opp().next();

    auto reachedLastEdge = false;

    // TODO Improve this looping
    while (!reachedLastEdge) {
//    while (counter < 3) {

        sides.emplace_back();

        auto reachedCorner = false;

        while (!reachedCorner && !reachedLastEdge) {

//            Log("Current: %d, Last: %d", currentHE.idx(), lastEdgeId);

//            currentEdge = currentHE.edge();

            sides.back().push_back(quadmesh.data(currentHE.to()).quadVertexData.patchParametrizations.at(patch));

            if (currentHE.edge().idx() == lastEdge.idx()) {
                reachedLastEdge = true;
            }

            if (quadmesh.data(currentHE.to()).quadVertexData.isCorner) {
                reachedCorner = true;
            } else {
                currentHE = currentHE.next().opp().next();
            }
        }

//        if (quadmesh.data(currentHE.from()).quadVertexData.isCorner) {
//            currentHE = currentHE.prev().opp();
//        }
        if (reachedCorner) {
            currentHE = currentHE.next();
        }

//        currentHE = currentHE.prev().opp();
    }

//    do {
//
//        sides.emplace_back();
//
//        do {
//
//            currentEdge = currentHE.edge();
//
//            sides.back().push_back(quadmesh.data(currentHE.to()).quadVertexData.patchParametrizations[patch]);
//
//            currentHE = currentHE.next().opp().next();
//
//        } while (!quadmesh.data(currentHE.from()).quadVertexData.isCorner);
//
//        currentHE = currentHE.prev().opp();
//
//        Log("Current: %d, Last: %d", currentEdge.idx(), lastEdgeId);
//
//    } while (currentEdge.idx() != lastEdgeId);

    return sides;
}

std::vector<int> skbar::Requadrangulator::FindCommonPatches(const skbar::SketchVertex &sv0,
                                                            const skbar::SketchVertex &sv1) {

    std::set<int> sv0Patches;
    std::set<int> sv1Patches;

    std::vector<int> result;
//    result.reserve(max(sv0.ParametricPositionsByPatch().size(). sv1.ParametricPositionsByPatch().size()));

    for (const auto &position : sv0.ParametricPositionsByPatch()) {
        sv0Patches.insert(position.first);
    }

    for (const auto &position : sv1.ParametricPositionsByPatch()) {
        sv1Patches.insert(position.first);
    }

    std::set_intersection(sv0Patches.begin(), sv0Patches.end(), sv1Patches.begin(), sv1Patches.end(),
                          std::back_inserter(result));

    return result;
}

int skbar::Requadrangulator::SplitQuadEdge(unsigned int edgeId, const skbar::Vec3f &position) {
    auto &quadmesh = dynamic_cast<OPQuadMesh &>(editableMesh->GetQuad()).Get();

//    const auto pos = utils::ToOPVector(position);
//
//    auto v = quadmesh.add_vertex(pos);
//
////    quadmesh.data(v).
////
////    quadmesh.split_edge(quadmesh.edge_handle(edgeId), OpenMesh::VertexHandle(v.idx()));
//    return static_cast<unsigned int>(v.idx());
    return -1;
}

void skbar::Requadrangulator::DeletePatchFaces(int patch) {
    auto &quadmesh = dynamic_cast<OPQuadMesh &>(editableMesh->GetQuad()).Get();

    for (const auto &face : quadmesh.faces()) {

        const auto fPatch = quadmesh.data(face).quadFaceData.patchId;

        if (fPatch == patch) {
            quadmesh.delete_face(face, true);
        }
    }
}

int skbar::Requadrangulator::AddQuadVertex(QuadMesh &mesh, const skbar::Vec3f &position) {
    auto &quadmesh = dynamic_cast<OPQuadMesh &>(mesh).Get();

    const OpenMesh::Vec3f pos = utils::ToOPVector(position);

    const auto v = quadmesh.add_vertex(pos);

    return v.idx();
}

int skbar::Requadrangulator::AddSketchVertexToQuadMesh(const skbar::SketchVertex &sketchVertex) {
    auto &quadmesh = dynamic_cast<OPQuadMesh &>(editableMesh->GetQuad()).Get();

    // Add vertex to quadmesh
    const auto newVertexId = AddQuadVertex(editableMesh->GetQuad(), sketchVertex.Position());
    const auto newVertex = OpenMesh::VertexHandle(newVertexId);

    auto &quadVertexData = quadmesh.data(newVertex).quadVertexData;

    quadVertexData.isCorner = false;
    quadVertexData.patchParametrizations = sketchVertex.ParametricPositionsByPatch();

    return newVertexId;
}

int skbar::Requadrangulator::AddQuadFace(skbar::QuadMesh &mesh, const std::vector<int> &verticesId) {

    auto &quadmesh = dynamic_cast<OPQuadMesh &>(mesh).Get();

    std::vector<OpenMesh::VertexHandle> vertices;

    for (const auto& vId : verticesId) {
        vertices.emplace_back(vId);
    }

    const auto face = quadmesh.add_face(vertices);

    return face.idx();
}

std::map<OpenMesh::SmartVertexHandle, OpenMesh::SmartVertexHandle>
skbar::Requadrangulator::MapNewPatchToQuadMesh(skbar::QuadMesh &mesh, const skbar::QuadMesh &newPatch,
                                               const std::vector<int> &borderVertices) {

    auto &quadmesh = dynamic_cast<OPQuadMesh &>(mesh).Get();
    const auto &patch = dynamic_cast<const OPQuadMesh &>(newPatch).Get();

    std::map<OpenMesh::SmartVertexHandle, OpenMesh::SmartVertexHandle> result;

    for (const auto &v  : patch.vertices()) {
        auto indexOnPositionVector = patch.data(v).patchgen.indexOnPositionVector;

        if (indexOnPositionVector < 0) {
            const auto pos = patch.point(v);

            const auto newVertexId = AddQuadVertex(utils::ToStdVector(pos));

            result[v] = OpenMesh::SmartVertexHandle(newVertexId, &quadmesh);
        } else {
            result[v] = OpenMesh::SmartVertexHandle(borderVertices.at(indexOnPositionVector), &quadmesh);
        }
    }

    return result;
}

void skbar::Requadrangulator::CreateNewFacesOnQuadMesh(
        skbar::QuadMesh &mesh, int patch, const skbar::QuadMesh &newPatch,
        const std::map<OpenMesh::SmartVertexHandle, OpenMesh::SmartVertexHandle> &newPatchToQuadMesh) {

    auto &quadmesh = dynamic_cast<OPQuadMesh &>(mesh).Get();
    const auto &patchTopology = dynamic_cast<const OPQuadMesh &>(newPatch).Get();

    for (const auto &face : patchTopology.faces()) {
        std::vector<int> verticesOnQuadmeshFace;

        for (const auto &v : face.vertices().to_vector()) {
            verticesOnQuadmeshFace.push_back(newPatchToQuadMesh.at(v).idx());
        }

        auto newFaceId = AddQuadFace(mesh, verticesOnQuadmeshFace);
        auto newFace = quadmesh.face_handle(newFaceId);

        quadmesh.data(newFace).quadFaceData.patchId = patch;
        quadmesh.data(newFace).quadFaceData.id = newFaceId;
    }
}