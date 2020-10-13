//
// Created by cassiano on 09/09/2020.
//


#include "requadrangulator.h"

#include "opsketch.h"
#include "opquadmesh.h"
#include "optrimesh.h"
#include "patchquadrangulator.h"
#include "vector.h"

#include "utils/openmesh.h"
#include "utils/triangleutils.h"
#include "utils/debug.h"

#include <queue>
#include <sstream>
#include <algorithm>
#include <iterator>

#include <Eigen/Sparse>

skbar::Requadrangulator::Requadrangulator(skbar::EditableMesh *_mesh) : editableMesh(_mesh) {

}

void skbar::Requadrangulator::RequadrangulateAll() {

    // TODO Check if sketch is closed

    auto affectedPatches = FindAffectedPatches();

    for (const auto &entry: affectedPatches) {

        std::stringstream ss;

        for (const auto &inOut: entry.second) {
            ss << "(" << inOut.in << ", " << inOut.out << ") ";
        }

        std::string tuples = ss.str();

        Log("%d: %s", entry.first, tuples.c_str());
    }

//    for (const auto &entry : affectedPatches) {
//        RequadrangulatePatch(entry.first, entry.second);
//    }

    if (affectedPatches.size() > 1) {
        RequadrangulatePatches(*editableMesh, affectedPatches);
    }
}

std::map<int, std::vector<skbar::Requadrangulator::InOutSKVIndex>>
skbar::Requadrangulator::FindAffectedPatches() const {

    std::map<int, std::vector<skbar::Requadrangulator::InOutSKVIndex>> result;

    auto &sketch = dynamic_cast<OPSketch &>(editableMesh->GetSketch());

    auto &quadmesh = dynamic_cast<OPQuadMesh &>(editableMesh->GetQuad()).Get();
    auto &trimesh = dynamic_cast<OPTriMesh &>(editableMesh->GetTri()).Get();

    auto firstPatch = -1;
    auto currentPatch = -1;

    int inSKVOnCurrentPatch = -1;
    int outSKVOnFirstPatch = -1;

    bool firstPatchAlreadyAdded = false;

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

            const auto &triEdgeData = trimesh.data(OpenMesh::EdgeHandle(sv.Pointer())).triEdgeData;

            if (triEdgeData.IsQuadEdge()) {
                const auto edge = OpenMesh::SmartEdgeHandle(triEdgeData.quadEdgeId, &quadmesh);

                if (quadmesh.data(edge).quadEdgeData.patchEdge) {
                    const auto f0 = edge.h0().face();
                    const auto f1 = edge.h1().face();

                    const auto newInSKVOnCurrentPatch = static_cast<int>(i);

                    const auto inOut = InOutSKVIndex{
                            inSKVOnCurrentPatch, // in
                            newInSKVOnCurrentPatch // out
                    };

                    // Don't add if is the first patch
                    if (inSKVOnCurrentPatch != -1) {
                        if (result.find(currentPatch) != result.end()) {
                            result.at(currentPatch).push_back(inOut);
                        } else {
                            // Element doesn't exist
                            result[currentPatch] = std::vector{inOut};
                        }
                    } else {
                        outSKVOnFirstPatch = newInSKVOnCurrentPatch;
                    }

                    const auto faceOnNewPatch = (quadmesh.data(f0).quadFaceData.patchId == currentPatch) ? f1 : f0;

                    inSKVOnCurrentPatch = newInSKVOnCurrentPatch;

                    // Update current because its the end of the old and the start of the new patch
                    currentPatch = quadmesh.data(faceOnNewPatch).quadFaceData.patchId;
                }
            }
        }
    }

    const auto inOut = InOutSKVIndex{
            inSKVOnCurrentPatch, // in
            outSKVOnFirstPatch // out
    };

    // Add last in to first out
    if (result.find(currentPatch) != result.end()) {
        result.at(currentPatch).push_back(inOut);
    } else {
        // Element doesn't exist
        result[currentPatch] = std::vector{inOut};
    }

    return result;
}

void skbar::Requadrangulator::RequadrangulatePatch(int patch,
                                                   const std::vector<skbar::Requadrangulator::InOutSKVIndex> &inOuts) {

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
skbar::Requadrangulator::RequadrangulatePatchWithoutHole(int patch,
                                                         const std::vector<skbar::Requadrangulator::InOutSKVIndex> &inOuts) {

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

                const auto newVertexId = AddSketchVertexToQuadMesh(editableMesh->GetQuad(), sketchVertexToAdd);

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

                            const auto newVertexId = AddSketchVertexToQuadMesh(editableMesh->GetQuad(), sketchVertex);

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

            std::map<int, int> newPatchToQuadMesh;

            {
                auto newVerticesMap = AddNewVerticesToQuad(editableMesh->GetQuad(), editableMesh->GetTri(),
                                                           newPolygonCCW, patch);

                auto oldVerticesMap = MapNewPatchToQuadMesh(editableMesh->GetQuad(), newPolygonCCW,
                                                            borderVertices);


                newPatchToQuadMesh.merge(oldVerticesMap);
                newPatchToQuadMesh.merge(newVerticesMap);
            }

            DeletePatchFaces(editableMesh->GetQuad(), patch);
            CreateNewFacesOnQuadMesh(editableMesh->GetQuad(), patch, newPolygonCCW, newPatchToQuadMesh);

            quadmesh.update_normals();

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

void skbar::Requadrangulator::DeletePatchFaces(QuadMesh &mesh, int patch) {
    auto &quadmesh = dynamic_cast<OPQuadMesh &>(mesh).Get();

    for (const auto &face : quadmesh.faces()) {

        const auto fPatch = quadmesh.data(face).quadFaceData.patchId;

        if (fPatch == patch) {
            quadmesh.delete_face(face, true);
        }
    }
}

void skbar::Requadrangulator::DeleteAllPatchFaces(skbar::QuadMesh &mesh, std::set<int> patches) {
    auto &quadmesh = dynamic_cast<OPQuadMesh &>(mesh).Get();

    for (const auto &face : quadmesh.faces()) {

        const auto fPatch = quadmesh.data(face).quadFaceData.patchId;

        if (patches.find(fPatch) != patches.end()) {
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

int skbar::Requadrangulator::AddSketchVertexToQuadMesh(QuadMesh &mesh, const skbar::SketchVertex &sketchVertex) {
    auto &quadmesh = dynamic_cast<OPQuadMesh &>(mesh).Get();

    // Add vertex to quadmesh
    const auto newVertexId = AddQuadVertex(mesh, sketchVertex.Position());
    const auto newVertex = OpenMesh::VertexHandle(newVertexId);

    auto &quadVertexData = quadmesh.data(newVertex).quadVertexData;

    quadVertexData.isCorner = false;
    quadVertexData.patchParametrizations = sketchVertex.ParametricPositionsByPatch();

    return newVertexId;
}

int skbar::Requadrangulator::AddSketchVertexToQuadMesh2(QuadMesh &mesh, const skbar::SketchVertex &sketchVertex) {
    auto &quadmesh = dynamic_cast<OPQuadMesh &>(mesh).Get();

    // Add vertex to quadmesh
    const auto newVertexId = AddQuadVertex(mesh, sketchVertex.Position());
    const auto newVertex = OpenMesh::VertexHandle(newVertexId);

    auto &quadVertexData = quadmesh.data(newVertex).quadVertexData;

    quadVertexData.isCorner = false;
    quadVertexData.patchParametrizations = sketchVertex.ParametricPositionsByPatch();

    if (sketchVertex.Type() == SketchVertex::EType::EDGE) {
        quadmesh.split_edge(OpenMesh::EdgeHandle(sketchVertex.Pointer()), newVertex);
    }

    return newVertexId;
}

void skbar::Requadrangulator::SplitQuadEdges(skbar::QuadMesh &mesh, const skbar::Sketch &sketch,
                                             const std::vector<int> &sketchVertexOnMesh,
                                             const std::vector<bool> &mustSplitEdge) {

    auto &quadmesh = dynamic_cast<OPQuadMesh &>(mesh).Get();

    assert(((sketch.Size() == sketchVertexOnMesh.size()) && (sketch.Size() == mustSplitEdge.size())) &&
           "Sizes don't match!");

    for (std::size_t i = 0; i < sketch.Size(); i++) {
        const auto &skv = sketch.Data().at(i);

        if (mustSplitEdge.at(i)) {
            const auto e = OpenMesh::EdgeHandle(skv.Pointer());
            const auto v = OpenMesh::VertexHandle(sketchVertexOnMesh.at(i));

            quadmesh.split_edge(e, v);
        }
    }
}

int skbar::Requadrangulator::AddQuadFace(skbar::QuadMesh &mesh, const std::vector<int> &verticesId) {

    auto &quadmesh = dynamic_cast<OPQuadMesh &>(mesh).Get();

    std::vector<OpenMesh::VertexHandle> vertices;

    for (const auto &vId : verticesId) {
        vertices.emplace_back(vId);
    }

    const auto face = quadmesh.add_face(vertices);

    return face.idx();
}

std::map<int, int>
skbar::Requadrangulator::MapNewPatchToQuadMesh(skbar::QuadMesh &mesh, const skbar::QuadMesh &newPatch,
                                               const std::vector<int> &borderVertices) {

    auto &quadmesh = dynamic_cast<OPQuadMesh &>(mesh).Get();
    const auto &patch = dynamic_cast<const OPQuadMesh &>(newPatch).Get();

    std::map<int, int> result;

    for (const auto &v  : patch.vertices()) {
        auto indexOnPositionVector = patch.data(v).patchgen.indexOnPositionVector;

        if (indexOnPositionVector >= 0) {
            result[v.idx()] = borderVertices.at(indexOnPositionVector);
        }
    }

    return result;
}

void skbar::Requadrangulator::CreateNewFacesOnQuadMesh(
        skbar::QuadMesh &mesh, int patch, const skbar::QuadMesh &newPatch,
        const std::map<int, int> &newPatchToQuadMesh) {

    auto &quadmesh = dynamic_cast<OPQuadMesh &>(mesh).Get();
    const auto &patchTopology = dynamic_cast<const OPQuadMesh &>(newPatch).Get();

    for (const auto &face : patchTopology.faces()) {
        std::vector<int> verticesOnQuadmeshFace;

        for (const auto &v : face.vertices().to_vector()) {
            verticesOnQuadmeshFace.push_back(newPatchToQuadMesh.at(v.idx()));
        }

        auto newFaceId = AddQuadFace(mesh, verticesOnQuadmeshFace);
        auto newFace = quadmesh.face_handle(newFaceId);

        quadmesh.data(newFace).quadFaceData.patchId = patch;
        quadmesh.data(newFace).quadFaceData.id = newFaceId;
    }
}

std::pair<int, int>
skbar::Requadrangulator::FindQuadTriangles(const skbar::QuadMesh &aQuadMesh, const skbar::TriMesh &aTriMesh,
                                           int quadId) {

    std::pair<int, int> result{-1, -1};

    const auto &quadmesh = dynamic_cast<const OPQuadMesh &>(aQuadMesh).Get();
    const auto &trimesh = dynamic_cast<const OPTriMesh &>(aTriMesh).Get();

    if (quadId >= 0) {

        const auto face = OpenMesh::SmartFaceHandle(quadId, &trimesh);

        assert((trimesh.data(face).triFaceData.quadFaceId == quadId) && "Tri and quad mesh are not related!");

        result.first = quadId;

        for (const auto &he : face.halfedges()) {

            const auto &oppFace = he.opp().face();
            const auto oppQuadId = trimesh.data(oppFace).triFaceData.quadFaceId;

            if ((oppFace != face) && (oppQuadId == quadId)) {
                result.second = oppQuadId;

                break;
            }
        }
    }

    return result;
}

skbar::Vec3f
skbar::Requadrangulator::UnparametrizeVertex(const skbar::QuadMesh &aQuadMesh, const skbar::TriMesh &aTriMesh,
                                             const int patch,
                                             const Vec2f parametricPosition) {

    // TODO Add check if parametricPosition is between 0 and 1

    const auto &quadmesh = dynamic_cast<const OPQuadMesh &>(aQuadMesh).Get();
    const auto &trimesh = dynamic_cast<const OPTriMesh &>(aTriMesh).Get();

    const auto &patchFaces = aQuadMesh.GetPatch(patch);

    assert(patchFaces.has_value() && "Invalid patch!");

    const auto patchHeight = patchFaces.value().size();
    const auto patchWidth = patchFaces.value().front().size();

    const auto line = std::max(static_cast<int>(ceil(parametricPosition[0] * patchHeight)) - 1, 0);
    const auto column = std::max(static_cast<int>(ceil(parametricPosition[1] * patchWidth)) - 1, 0);

    const auto faceId = patchFaces.value().at(line).at(column);

    const auto[faceId1, faceId2] = FindQuadTriangles(aQuadMesh, aTriMesh, faceId);

    Vec3f newPosition;

    {
        auto tempPos = GetUnparametrizedPosition(aQuadMesh, aTriMesh, faceId1, parametricPosition);

        if (tempPos) {
            newPosition = tempPos.value();
        } else {

            tempPos = GetUnparametrizedPosition(aQuadMesh, aTriMesh, faceId2, parametricPosition);

            assert(tempPos.has_value() && "Vertex are not inside the triangle!");

            newPosition = tempPos.value();
        }
    }

    return newPosition;
}

std::optional<skbar::Vec3f>
skbar::Requadrangulator::GetUnparametrizedPosition(const skbar::QuadMesh &aQuadMesh, const skbar::TriMesh &triMesh,
                                                   const int trifaceId, const skbar::Vec2f &parametricPosition) {

    std::optional<Vec3f> result;

    const auto &quadmesh = dynamic_cast<const OPQuadMesh &>(aQuadMesh).Get();
    const auto &trimesh = dynamic_cast<const OPTriMesh &>(triMesh).Get();

    const auto f1 = OpenMesh::SmartFaceHandle(trifaceId, &trimesh);

    const auto patch = quadmesh.data(
            OpenMesh::FaceHandle(trimesh.data(f1).triFaceData.quadFaceId)).quadFaceData.patchId;

    const auto vertices = f1.vertices().to_vector();

    // The vertices of tri and quad mesh are on same indices
    const auto pp0 = quadmesh.data(
            OpenMesh::VertexHandle(vertices[0].idx())).quadVertexData.patchParametrizations.at(patch);

    const auto pp1 = quadmesh.data(
            OpenMesh::VertexHandle(vertices[1].idx())).quadVertexData.patchParametrizations.at(patch);

    const auto pp2 = quadmesh.data(
            OpenMesh::VertexHandle(vertices[2].idx())).quadVertexData.patchParametrizations.at(patch);

    const auto pp = parametricPosition;

    const auto barycentricPosition = utils::GetBarycentricCoordinate(pp, pp0, pp1, pp2);

    if (barycentricPosition) {

        const auto p0 = utils::ToStdVector(quadmesh.point(OpenMesh::VertexHandle(vertices[0].idx())));
        const auto p1 = utils::ToStdVector(quadmesh.point(OpenMesh::VertexHandle(vertices[1].idx())));
        const auto p2 = utils::ToStdVector(quadmesh.point(OpenMesh::VertexHandle(vertices[2].idx())));

        const auto bp0 = barycentricPosition.value().at(0);
        const auto bp1 = barycentricPosition.value().at(1);
        const auto bp2 = barycentricPosition.value().at(2);

        const Vec3f newPosition = Sum(Sum(Mul(p0, bp0), Mul(p1, bp1)), Mul(p2, bp2));

        result = newPosition;
    }

    return result;
}

std::map<int, int>
skbar::Requadrangulator::AddNewVerticesToQuad(skbar::QuadMesh &aQuadMesh, const skbar::TriMesh &aTriMesh,
                                              const skbar::QuadMesh &aPatch, const int patchId) {

    std::map<int, int> result;

    const auto &quadmesh = dynamic_cast<OPQuadMesh &>(aQuadMesh).Get();
    const auto &trimesh = dynamic_cast<const OPTriMesh &>(aTriMesh).Get();

    const auto &patch = dynamic_cast<const OPQuadMesh &>(aPatch).Get();

    for (const auto &v : patch.vertices()) {
        auto indexOnPositionVector = patch.data(v).patchgen.indexOnPositionVector;

        const auto &laplacian = patch.data(v).laplacian;

        if (!laplacian.fixed) {
            const auto tempPos = patch.point(v);

            const Vec2f parametricPosition{tempPos[0], tempPos[1]};

            const auto pos = UnparametrizeVertex(aQuadMesh, aTriMesh, patchId, parametricPosition);

            const auto newVertexId = AddQuadVertex(aQuadMesh, pos);

            result[v.idx()] = newVertexId;
        }
    }

    return result;
}

std::map<int, std::size_t> skbar::Requadrangulator::FindEdgesToCheck(
        const skbar::EditableMesh &editableMesh,
        int patch,
        const std::vector<skbar::Requadrangulator::InOutSKVIndex> &inOuts) {

    std::map<int, std::size_t> result;

    const auto &sketch = dynamic_cast<const OPSketch &>(editableMesh.GetSketch());

    const auto &quadmesh = dynamic_cast<const OPQuadMesh &>(editableMesh.GetQuad()).Get();
    const auto &trimesh = dynamic_cast<const OPTriMesh &>(editableMesh.GetTri()).Get();

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

        result[inEdgeId] = i;
        result[outEdgeId] = i;
    }
}

//skbar::Requadrangulator::PolygonData
//skbar::Requadrangulator::WalkOnBorder(const EditableMesh &editableMesh, const int patch,
//                                      const int heIndex,
//                                      const std::vector<InOutSKVIndex> &inOutsOnPatch,
//                                      const std::map<int, std::size_t> &edgesToCheck,
//                                      std::map<int, bool> &halfEdgesChecked) {
//
//    PolygonData result;
//
//    const auto &sketch = dynamic_cast<const OPSketch &>(editableMesh.GetSketch());
//
//    const auto &quadmesh = dynamic_cast<const OPQuadMesh &>(editableMesh.GetQuad()).Get();
//    const auto &trimesh = dynamic_cast<const OPTriMesh &>(editableMesh.GetTri()).Get();
//
//    const auto[inId, outId] = inOutsOnPatch[inOutId];
//
//    // Used to add sketch vertices in reverse order if is true
//    const auto startingFromOut = (firstHE.edge().idx() == sketch.Data().at(outId).Pointer());
//
//    const auto &lastEdge = startingFromOut
//                           ? OpenMesh::SmartEdgeHandle(sketch.Data().at(inId).Pointer(), &quadmesh)
//                           : OpenMesh::SmartEdgeHandle(sketch.Data().at(outId).Pointer(), &quadmesh);
//
////    auto currentHE = firstHE;
////
////    // TODO Check if in == out
////    auto reachedLastEdge = false;
////
////    // Add the first sketch vertex position
////    {
////        const auto sKVToAddPosition = startingFromOut ? outId : inId;
////
////        const auto &sketchVertexToAdd = sketch.Data().at(sKVToAddPosition);
////
////        borderVertexPositions.push_back(sketchVertexToAdd.ParametricPositionsByPatch().at(patch));
////
////        int sKVOnMesh = traceData.sketchVertexOnMesh[sKVToAddPosition];
////
////        // If vertex not add yet, create it on mesh and set its position
////        if (sKVOnMesh == -1) {
////            sKVOnMesh = AddSketchVertexToQuadMesh(editableMesh.GetQuad(), sketchVertexToAdd);
////
//////            traceData.sketchVertexOnMesh[sKVToAddPosition] = sKVOnMesh;
//////            traceData.mustSplitEdge[sKVToAddPosition] = true;
////        }
////
////        borderVertices.push_back(sKVOnMesh);
////    }
////
////    // Walk on patch edge to find the borders
////    while (!reachedLastEdge) {
////
////        auto reachedCorner = quadmesh.data(currentHE.to()).quadVertexData.isCorner;
////
////        auto countEdges = 1;
////
////        while (!reachedCorner) {
////
////            if (currentHE.edge() == lastEdge) {
////                reachedLastEdge = true;
////                break;
////            }
////
////            countEdges++;
////
////            const auto &parametricPosition = quadmesh.data(
////                    currentHE.to()).quadVertexData.patchParametrizations[patch];
////
////            borderVertexPositions.push_back(parametricPosition);
////            borderVertices.push_back(currentHE.to().idx());
////
////            currentHE = currentHE.next().opp().next();
////
////            reachedCorner = quadmesh.data(currentHE.to()).quadVertexData.isCorner;
////        }
////
////        if (!reachedLastEdge) {
////            const auto &parametricPosition = quadmesh.data(
////                    currentHE.to()).quadVertexData.patchParametrizations[patch];
////
////            // Don't forget the last vertex
////            borderVertexPositions.push_back(parametricPosition);
////            borderVertices.push_back(currentHE.to().idx());
////
////            currentHE = currentHE.next();
////        }
////
////        edgesBySide.push_back(countEdges);
////    }
////
////    auto countEdges = 0;
////    std::vector<skbar::Vec2f> sketchPositions;
////
////    auto next = [](auto i, auto size, bool reverse) -> auto {
////        return reverse ? ((i + size - 1) % size) : ((i + 1) % size);
////    };
////
////    // Add last side (sketch)
////    {
////        const auto first = startingFromOut ? inId : outId;
////        const auto last = startingFromOut ? outId : inId;
////
////        for (std::size_t i = first; i != static_cast<std::size_t>(last);
////             i = next(i, sketch.Size(), !startingFromOut)) {
////
////            const auto &sketchVertex = sketch.Data().at(i);
////
////            // TODO Check for vertices on a face
////            if (sketchVertex.Type() == skbar::SketchVertex::EType::EDGE) {
////                const auto triVertex = OpenMesh::EdgeHandle(sketchVertex.Pointer());
////
////                const auto isQuadEdge = trimesh.data(triVertex).triEdgeData.IsQuadEdge();
////
////                if (isQuadEdge) {
////                    countEdges++;
////
////                    sketchPositions.push_back(sketchVertex.ParametricPositionsByPatch().at(patch));
////
////                    int sKVOnMesh = traceData.sketchVertexOnMesh[i];
////
////                    // If vertex not add yet, create it on mesh and set its position
////                    if (sKVOnMesh == -1) {
////                        sKVOnMesh = AddSketchVertexToQuadMesh(editableMesh.GetQuad(), sketchVertex);
////
//////                        traceData.sketchVertexOnMesh[i] = sKVOnMesh;
//////                        traceData.mustSplitEdge[i] = true;
////                    }
////
////                    borderVertices.push_back(sKVOnMesh);
////                }
////
////
////            }
////        }
////    }
//}

skbar::Requadrangulator::TraceData skbar::Requadrangulator::TracePolygons(
        skbar::EditableMesh &editableMesh,
        const std::map<int, std::vector<skbar::Requadrangulator::InOutSKVIndex>> &inOutsByPatchMap) {

    TraceData traceData;

    auto &sketch = dynamic_cast<OPSketch &>(editableMesh.GetSketch());

    auto &quadmesh = dynamic_cast<OPQuadMesh &>(editableMesh.GetQuad()).Get();
    auto &trimesh = dynamic_cast<OPTriMesh &>(editableMesh.GetTri()).Get();

    for (const auto &entry: inOutsByPatchMap) {

        const auto &patch = entry.first;
        const auto &inOutsOnPatch = entry.second;

        std::vector<int> edgesBySide;
        std::vector<Vec2f> borderVertexPositions;
        std::vector<int> borderVertices;

        const auto halfEdgesToCheck = FindEdgesToCheck(editableMesh, patch, inOutsOnPatch);

        std::map<int, bool> halfEdgesChecked;

        for (const auto &entry : halfEdgesToCheck) {
            halfEdgesChecked.emplace(entry.first, false);
        }

        for (const auto &entry : halfEdgesToCheck) {
            const auto &firstHEId = entry.first;

            const auto inOutId = entry.second;
            const auto checked = halfEdgesChecked.at(entry.first);

            const auto firstHE = OpenMesh::SmartHalfedgeHandle(firstHEId, &quadmesh);

            if (!checked) {
                const auto[inId, outId] = inOutsOnPatch[inOutId];

                // Used to add sketch vertices in reverse order if is true
                const auto startingFromOut = (firstHE.edge().idx() == sketch.Data().at(outId).Pointer());

                const auto &lastEdge = startingFromOut
                                       ? OpenMesh::SmartEdgeHandle(sketch.Data().at(inId).Pointer(), &quadmesh)
                                       : OpenMesh::SmartEdgeHandle(sketch.Data().at(outId).Pointer(), &quadmesh);

                auto currentHE = firstHE;

                // TODO Check if in == out
                auto reachedLastEdge = false;

                // Add the first sketch vertex position
                {
                    const auto sKVToAddPosition = startingFromOut ? outId : inId;

                    const auto &sketchVertexToAdd = sketch.Data().at(sKVToAddPosition);

                    borderVertexPositions.push_back(sketchVertexToAdd.ParametricPositionsByPatch().at(patch));

                    int sKVOnMesh = traceData.sketchVertexOnMesh[sKVToAddPosition];

                    // If vertex not add yet, create it on mesh and set its position
                    if (sKVOnMesh == -1) {
                        sKVOnMesh = AddSketchVertexToQuadMesh(editableMesh.GetQuad(), sketchVertexToAdd);

                        traceData.sketchVertexOnMesh[sKVToAddPosition] = sKVOnMesh;
                        traceData.mustSplitEdge[sKVToAddPosition] = true;
                    }

                    borderVertices.push_back(sKVOnMesh);
                }

                // Walk on patch edge to find the borders
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

                // Add last side (sketch)
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

                                int sKVOnMesh = traceData.sketchVertexOnMesh[i];

                                // If vertex not add yet, create it on mesh and set its position
                                if (sKVOnMesh == -1) {
                                    sKVOnMesh = AddSketchVertexToQuadMesh(editableMesh.GetQuad(), sketchVertex);

                                    traceData.sketchVertexOnMesh[i] = sKVOnMesh;
                                    traceData.mustSplitEdge[i] = true;
                                }

                                borderVertices.push_back(sKVOnMesh);
                            }


                        }
                    }
                }

                edgesBySide.push_back(countEdges);

                borderVertexPositions.insert(borderVertexPositions.end(), sketchPositions.begin(),
                                             sketchPositions.end());

            }

            // Set HE as checked
            halfEdgesChecked.at(firstHEId) = true;

            traceData.polygonsData.emplace(patch, PolygonData{
                    edgesBySide, // edgesBySide
                    borderVertexPositions, // borderVertexPositions
                    borderVertices // borderVertices
            });

            // TODO Check other HEs
            break;
        }

        // TODO Check other patches
        break;
    }

    return traceData;
}


void
skbar::Requadrangulator::RequadrangulatePatches(skbar::EditableMesh &editableMesh,
                                                const std::map<int, std::vector<
                                                        skbar::Requadrangulator::InOutSKVIndex>> &inOutsByPatchMap) {

    auto &sketch = dynamic_cast<OPSketch &>(editableMesh.GetSketch());

    auto &quadmesh = dynamic_cast<OPQuadMesh &>(editableMesh.GetQuad()).Get();
    auto &trimesh = dynamic_cast<OPTriMesh &>(editableMesh.GetTri()).Get();

    const auto traceData = TracePolygons(editableMesh, inOutsByPatchMap);

    const auto &dataByPolygonInPatch = traceData.polygonsData;
    const auto &sketchVertexOnMesh = traceData.sketchVertexOnMesh;
    const auto &mustSplitEdge = traceData.mustSplitEdge;

    // Delete all faces first
    {
        std::set<int> temp;

        for (const auto &entry: dataByPolygonInPatch) {

            temp.insert(entry.first);

            DeleteAllPatchFaces(editableMesh.GetQuad(), temp);
        }
    }

    // Then split the edges
    SplitQuadEdges(editableMesh.GetQuad(), sketch, sketchVertexOnMesh, mustSplitEdge);

    // Then create create the new faces
    for (const auto &entry: dataByPolygonInPatch) {
        const auto patch = entry.first;

        const auto &edgesBySide = entry.second.edgesBySide;
        const auto &borderVertexPositions = entry.second.borderVertexPositions;
        const auto &borderVertices = entry.second.borderVertices;

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

//        newPolygonCCW.Save("./test_ccw.obj");
//        newPolygonCW.Save("./test_cw.obj");

        std::map<int, int> newPatchToQuadMesh;

        {
            auto newVerticesMap = AddNewVerticesToQuad(editableMesh.GetQuad(), editableMesh.GetTri(),
                                                       newPolygonCCW, patch);

            auto oldVerticesMap = MapNewPatchToQuadMesh(editableMesh.GetQuad(), newPolygonCCW,
                                                        borderVertices);


            newPatchToQuadMesh.merge(oldVerticesMap);
            newPatchToQuadMesh.merge(newVerticesMap);
        }

        CreateNewFacesOnQuadMesh(editableMesh.GetQuad(), patch, newPolygonCCW, newPatchToQuadMesh);
    }

    // Then clear everything

    quadmesh.update_normals();


//    // TODO Find a better place to do this
//    {
//        Eigen::VectorXi sides(edgesBySide.size());
//        std::vector<OpenMesh::Vec3d> parametricPositions;
//
//        for (std::size_t i = 0; i < edgesBySide.size(); i++) {
//            sides(i) = edgesBySide[i];
//        }
//
//        const auto borderSize = borderVertexPositions.size();
//
//        for (std::size_t i = 0; i < borderSize; i++) {
//            const auto position = borderVertexPositions[i];
//
//            parametricPositions.emplace_back(position[0], position[1], 0);
//        }
//
//        const auto newPolygonCCW = PatchQuadrangulator::Quadrangulate(sides, parametricPositions, false);
//        const auto newPolygonCW = PatchQuadrangulator::Quadrangulate(sides, parametricPositions, true);
//
//        newPolygonCCW.Save("./test_ccw.obj");
//        newPolygonCW.Save("./test_cw.obj");
//
//        std::map<int, int> newPatchToQuadMesh;
//
//        {
//            auto newVerticesMap = AddNewVerticesToQuad(editableMesh.GetQuad(), editableMesh.GetTri(),
//                                                       newPolygonCCW, patch);
//
//            auto oldVerticesMap = MapNewPatchToQuadMesh(editableMesh.GetQuad(), newPolygonCCW,
//                                                        borderVertices);
//
//
//            newPatchToQuadMesh.merge(oldVerticesMap);
//            newPatchToQuadMesh.merge(newVerticesMap);
//        }
//
//        DeletePatchFaces(editableMesh.GetQuad(), patch);
//        CreateNewFacesOnQuadMesh(editableMesh.GetQuad(), patch, newPolygonCCW, newPatchToQuadMesh);
//
//        quadmesh.update_normals();
//
////            // TODO Save a copy before clean deleted vertices
////            quadmesh.garbage_collection();
//    }
}

std::map<int, bool> skbar::Requadrangulator::CutQuadWithSketch(skbar::EditableMesh &editableMesh) {

    std::map<int, bool> result;

    auto &sketch = dynamic_cast<OPSketch &>(editableMesh.GetSketch());

    auto &quadmesh = dynamic_cast<OPQuadMesh &>(editableMesh.GetQuad()).Get();
    auto &trimesh = dynamic_cast<OPTriMesh &>(editableMesh.GetTri()).Get();

    std::set<int> facesToDelete;

    const auto sketchSize = sketch.Size();

//    OpenMesh::VertexHandle lastAddedVertex(-1);

    std::map<size_t, int> addedSkv;

    int currentFace = dynamic_cast<OPTriMesh &>(editableMesh.GetTri()).GetQuadFaceId(sketch.Data()[0].Pointer());

    int currentSkvIndex = 0;

    // Skip the first face for now
    while (true) {

        const auto &skv = sketch.Data().at(currentSkvIndex);

        if (skv.Type() == SketchVertex::EType::EDGE) {

            const auto currentTriEdge = OpenMesh::EdgeHandle(skv.Pointer());
            const auto currentQuadEdge = OpenMesh::EdgeHandle(trimesh.data(currentTriEdge).triEdgeData.quadEdgeId);

            if (currentQuadEdge.is_valid() && quadmesh.data(currentQuadEdge).quadEdgeData.patchEdge) {
                break;
            }
        }

        currentSkvIndex++;
    }

    auto allFacesAdded = false;

    OpenMesh::VertexHandle inVertex;
    OpenMesh::VertexHandle outVertex;

    while (!allFacesAdded) {
        const auto &currentSkv = sketch.Data().at(currentSkvIndex);

        // First addition
        if (!inVertex.is_valid()) {
            const int id = AddSketchVertexToQuadMesh2(editableMesh.GetQuad(), currentSkv);

            inVertex = OpenMesh::VertexHandle(id);
        } else {

            if (currentSkv.Type() == SketchVertex::EType::EDGE) {

                const auto currentTriEdge = OpenMesh::EdgeHandle(currentSkv.Pointer());
                const auto currentQuadEdge = OpenMesh::EdgeHandle(trimesh.data(currentTriEdge).triEdgeData.quadEdgeId);

                if (currentQuadEdge.is_valid() && quadmesh.data(currentQuadEdge).quadEdgeData.patchEdge) {

                    const int newVertexId = AddSketchVertexToQuadMesh2(editableMesh.GetQuad(), currentSkv);

                    outVertex = OpenMesh::VertexHandle(newVertexId);

                    {
                        OpenMesh::SmartHalfedgeHandle firstHE;
//                        OpenMesh::SmartHalfedgeHandle firstHE;

                        for (const auto &he : OpenMesh::make_smart(inVertex, quadmesh).outgoing_halfedges()) {
                            if (he.face().idx() == currentFace) {
                                firstHE = he;
                            }
                        }

                        assert(firstHE.is_valid() && "Invalid half-edge!");

//                        OpenMesh::SmartHalfedgeHandle currentHE = firstHE;

                        // Find first side of new face
                        const auto findSide = [&quadmesh, currentFace](auto firstV, auto lastV)
                                -> std::vector<OpenMesh::SmartVertexHandle> {
                            std::vector<OpenMesh::SmartVertexHandle> result;

                            OpenMesh::SmartHalfedgeHandle firstHE;

                            // Find the first HE
                            for (const auto &he : OpenMesh::make_smart(firstV, quadmesh).outgoing_halfedges()) {
                                if (he.face().idx() == currentFace) {
                                    firstHE = he;

                                    result.push_back(firstHE.from());
                                }
                            }

                            OpenMesh::SmartHalfedgeHandle currentHE = firstHE;

                            while (currentHE.from() != lastV) {
                                result.push_back(currentHE.to());

                                currentHE = currentHE.next();
                            }

                            return result;
                        };

                        const auto side1 = findSide(inVertex, outVertex);
                        const auto side2 = findSide(outVertex, inVertex);

                        // Delete the face
                        quadmesh.delete_face(OpenMesh::FaceHandle(currentFace), false);

                        // Add the new face
                        quadmesh.add_face(side1);
                        quadmesh.add_face(side2);
                    }

                    // TODO Update current face
                }
            }
        }
    }

    return result;
}


