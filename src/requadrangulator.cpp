//
// Created by cassiano on 09/09/2020.
//


#include "requadrangulator.h"

#include "opsketch.h"
#include "opquadmesh.h"
#include "optrimesh.h"
#include "patchquadrangulator.h"

#include "utils/debug.h"

#include <algorithm>
#include <iterator>

skbar::Requadrangulator::Requadrangulator(skbar::EditableMesh *_mesh) : editableMesh(_mesh) {

}

void skbar::Requadrangulator::RequadrangulateAll() {
    // TODO Fix this
    RequadrangulatePatch(-1);
}

void skbar::Requadrangulator::RequadrangulatePatch(int patch) {
// TODO Implement this
    auto &sketch = dynamic_cast<OPSketch &>(editableMesh->GetSketch());

    auto &quadmesh = dynamic_cast<OPQuadMesh &>(editableMesh->GetQuad()).Get();
    auto &trimesh = dynamic_cast<OPTriMesh &>(editableMesh->GetTri()).Get();

    auto currentPatch = -1;

//    std::vector<Vec2f> points;
    std::vector<std::vector<Vec2f>> sides;
    int firstSketchVertex = -1;
    int lastSketchVertex = -1;

    // TODO Find a better place to do this
    for (unsigned int i = 0; i < sketch.Size(); i++) {

        const auto &sv = sketch.Data()[i];

        if (sv.Type() == SketchVertex::EType::FACE) {

            const auto faceId = trimesh.data(OpenMesh::FaceHandle(sv.Pointer())).triFaceData.quadFaceId;
            const auto face = OpenMesh::FaceHandle(faceId);

            currentPatch = quadmesh.data(face).quadFaceData.patchId;

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

                    firstSketchVertex = i;

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

    sides.insert(sides.end(), otherSides.begin(), otherSides.end());

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

            Log("Face: %d", quadmesh.data(currentHE.face()).quadFaceData.patchId);

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