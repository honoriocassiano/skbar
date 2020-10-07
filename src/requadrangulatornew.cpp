//
// Created by cassiano on 07/10/2020.
//

#include "requadrangulatornew.h"

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

void skbar::RequadrangulatorNew::RequadrangulateAll(EditableMesh &editableMesh) {
    auto affectedPatches = FindAffectedPatches(editableMesh);

    if (affectedPatches.size() > 1) {
        RequadrangulatePatches(editableMesh, affectedPatches);
    }
}

std::map<int, std::vector<skbar::RequadrangulatorNew::InOutSKVIndex>>
skbar::RequadrangulatorNew::FindAffectedPatches(const EditableMesh &editableMesh) {

    std::map<int, std::vector<skbar::RequadrangulatorNew::InOutSKVIndex>> result;

    const auto &sketch = dynamic_cast<const OPSketch &>(editableMesh.GetSketch());

    const auto &quadmesh = dynamic_cast<const OPQuadMesh &>(editableMesh.GetQuad()).Get();
    const auto &trimesh = dynamic_cast<const OPTriMesh &>(editableMesh.GetTri()).Get();

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

std::map<int, bool> skbar::RequadrangulatorNew::CutQuadMesh(skbar::EditableMesh &editableMesh) {

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

    auto allFacesAdded = false;

    OpenMesh::SmartVertexHandle inVertex;

    std::map<int, OpenMesh::SmartVertexHandle> mapSkvToVertex;

    std::set<int> addedFaces;

    int firstValidSkv = -1;

    while (!allFacesAdded) {
        const auto &currentSkv = sketch.Data().at(currentSkvIndex);

        // Find first side of new face
        const auto findSide = [currentFace](OpenMesh::SmartVertexHandle firstV,
                                            OpenMesh::SmartVertexHandle lastV)
                -> std::vector<OpenMesh::SmartVertexHandle> {

            std::vector<OpenMesh::SmartVertexHandle> result;

            OpenMesh::SmartHalfedgeHandle firstHE;

            // Find the first HE
            for (const auto &he : firstV.outgoing_halfedges()) {
                if (he.face().idx() == currentFace) {
                    firstHE = he;

                    result.push_back(firstHE.from());
                }
            }

            assert(firstHE.is_valid() && "Invalid half edge!");

            OpenMesh::SmartHalfedgeHandle currentHE = firstHE;

            while (currentHE.from() != lastV) {
                result.push_back(currentHE.to());

                currentHE = currentHE.next();
            }

            return result;
        };

        if (currentSkv.Type() == SketchVertex::EType::EDGE) {

            const auto currentTriEdge = OpenMesh::EdgeHandle(currentSkv.Pointer());
            const auto currentQuadEdge = OpenMesh::EdgeHandle(trimesh.data(currentTriEdge).triEdgeData.quadEdgeId);

            if (currentQuadEdge.is_valid()) {
                if (mapSkvToVertex.find(currentSkvIndex) == mapSkvToVertex.end()) {

                    const int newVertexId = AddSketchVertexToQuadMesh(editableMesh.GetQuad(), currentSkv);

                    result[newVertexId] = quadmesh.data(currentQuadEdge).quadEdgeData.patchEdge;

                    if (!inVertex.is_valid()) {

                        firstValidSkv = currentSkvIndex;

                        inVertex = OpenMesh::SmartVertexHandle(newVertexId, &quadmesh);
                        mapSkvToVertex[currentSkvIndex] = inVertex;

                        for (const auto &face : inVertex.faces()) {
                            if (face.idx() != currentFace) {
                                currentFace = face.idx();
                            }
                        }

                    } else {

                        const auto outVertex = OpenMesh::SmartVertexHandle(newVertexId, &quadmesh);

                        const auto side1 = findSide(inVertex, outVertex);
                        const auto side2 = findSide(outVertex, inVertex);

                        // Add the new faces
                        {
                            const auto oldData = quadmesh.data(OpenMesh::FaceHandle(currentFace)).quadFaceData;

                            // Delete the face
                            quadmesh.delete_face(OpenMesh::FaceHandle(currentFace), false);

                            // Add the new face
                            const auto newFaceId1 = quadmesh.add_face(side1);
                            const auto newFaceId2 = quadmesh.add_face(side2);

                            addedFaces.insert(newFaceId1.idx());
                            addedFaces.insert(newFaceId2.idx());

                            {
                                quadmesh.data(
                                        OpenMesh::FaceHandle(newFaceId1.idx())).quadFaceData.patchId = oldData.patchId;
                                quadmesh.data(
                                        OpenMesh::FaceHandle(newFaceId2.idx())).quadFaceData.patchId = oldData.patchId;
                            }
                        }

                        inVertex = outVertex;
                        mapSkvToVertex[currentSkvIndex] = inVertex;

                        for (const auto &face : inVertex.faces()) {
                            if ((face.idx() != currentFace) && (addedFaces.count(face.idx()) == 0)) {
                                currentFace = face.idx();
                            }
                        }
                    }
                } else {

                    const auto skvInsideFace = sketch.Data().at(0);

                    const auto vInsideFaceId = AddSketchVertexToQuadMesh(editableMesh.GetQuad(), skvInsideFace);
                    const auto vInsideFace = OpenMesh::SmartVertexHandle(vInsideFaceId, &quadmesh);

                    const auto outVertex = mapSkvToVertex[firstValidSkv];

                    auto side1 = findSide(inVertex, outVertex);
                    auto side2 = findSide(outVertex, inVertex);

                    side1.push_back(vInsideFace);
                    side2.push_back(vInsideFace);

                    const auto oldData = quadmesh.data(OpenMesh::FaceHandle(currentFace)).quadFaceData;

                    // Delete the face
                    quadmesh.delete_face(OpenMesh::FaceHandle(currentFace), false);

                    // Add the new face
                    const auto newFaceId1 = quadmesh.add_face(side1);
                    const auto newFaceId2 = quadmesh.add_face(side2);

                    addedFaces.insert(newFaceId1.idx());
                    addedFaces.insert(newFaceId2.idx());

                    {
                        quadmesh.data(
                                OpenMesh::FaceHandle(newFaceId1.idx())).quadFaceData.patchId = oldData.patchId;
                        quadmesh.data(
                                OpenMesh::FaceHandle(newFaceId2.idx())).quadFaceData.patchId = oldData.patchId;
                    }

                    // TODO Implement the last cut
                    allFacesAdded = true;
                }
            }
        }

        currentSkvIndex = (currentSkvIndex + 1) % sketchSize;

    }

    quadmesh.update_normals();

    return result;
}

void
skbar::RequadrangulatorNew::RequadrangulatePatches(skbar::EditableMesh &editableMesh,
                                                const std::map<int, std::vector<
                                                        skbar::RequadrangulatorNew::InOutSKVIndex>> &inOutsByPatchMap) {

    auto &sketch = dynamic_cast<OPSketch &>(editableMesh.GetSketch());

    auto &quadmesh = dynamic_cast<OPQuadMesh &>(editableMesh.GetQuad()).Get();
    auto &trimesh = dynamic_cast<OPTriMesh &>(editableMesh.GetTri()).Get();

    const auto traceData = TracePolygons(editableMesh, inOutsByPatchMap);

    CutQuadMesh(editableMesh);

//    // Delete all faces first
//    {
//        std::set<int> temp;
//
//        for (const auto &entry: dataByPolygonInPatch) {
//
//            temp.insert(entry.first);
//
//            DeleteAllPatchFaces(editableMesh.GetQuad(), temp);
//        }
//    }

//    // Then split the edges
//    SplitQuadEdges(editableMesh.GetQuad(), sketch, sketchVertexOnMesh, mustSplitEdge);

//    // Then create create the new faces
//    for (const auto &entry: dataByPolygonInPatch) {
//        const auto patch = entry.first;
//
//        const auto &edgesBySide = entry.second.edgesBySide;
//        const auto &borderVertexPositions = entry.second.borderVertexPositions;
//        const auto &borderVertices = entry.second.borderVertices;
//
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
////        newPolygonCCW.Save("./test_ccw.obj");
////        newPolygonCW.Save("./test_cw.obj");
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
//        CreateNewFacesOnQuadMesh(editableMesh.GetQuad(), patch, newPolygonCCW, newPatchToQuadMesh);
//    }

//    // Then clear everything
//
//    quadmesh.update_normals();
//
//
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

skbar::RequadrangulatorNew::TraceData skbar::RequadrangulatorNew::TracePolygons(
        skbar::EditableMesh &editableMesh,
        const std::map<int, std::vector<InOutSKVIndex>> &inOutsByPatchMap) {

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

std::map<int, std::size_t> skbar::RequadrangulatorNew::FindEdgesToCheck(
        const skbar::EditableMesh &editableMesh,
        int patch,
        const std::vector<InOutSKVIndex> &inOuts) {

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

int skbar::RequadrangulatorNew::AddQuadVertex(QuadMesh &mesh, const skbar::Vec3f &position) {
    auto &quadmesh = dynamic_cast<OPQuadMesh &>(mesh).Get();

    const OpenMesh::Vec3f pos = utils::ToOPVector(position);

    const auto v = quadmesh.add_vertex(pos);

    return v.idx();
}

int skbar::RequadrangulatorNew::AddSketchVertexToQuadMesh(QuadMesh &mesh, const skbar::SketchVertex &sketchVertex) {
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