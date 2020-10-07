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

//    if (affectedPatches.size() > 1) {
//        RequadrangulatePatches(editableMesh, affectedPatches);
//    }
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