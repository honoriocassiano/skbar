//
// Created by cassiano on 26/04/2020.
//

#include "patchtracer.h"

#include <algorithm>
#include <queue>

#include "utils/debug.h"

#define BFIND(container, value) (std::binary_search(container.begin(), container.end(), value))

void skbar::PatchTracer::Trace(skbar::OPQuadMesh &baseMesh) {

    auto &mesh = baseMesh.Get();

    const auto singularities = FindSingularities(mesh);

    const auto tracedEdges = GetEdges(mesh, singularities);

    for (const auto edge : tracedEdges) {
        mesh.data(OpenMesh::EdgeHandle(edge)).quadEdgeData.patchEdge = true;
    }

    // Count the patches that was not visited yet
    std::vector<int> patchesVisitedCount(singularities.size());

    for (std::size_t i = 0; i < singularities.size(); i++) {
        patchesVisitedCount[i] = mesh.valence(OpenMesh::VertexHandle(singularities[i]));
    }

    int currentPatch = 0;

    unsigned int faceToStartCheck = 0;

    while (faceToStartCheck < mesh.n_faces()) {

        const auto startF = mesh.face_handle(faceToStartCheck);

        std::queue<unsigned int> facesToCheck;

        facesToCheck.push(faceToStartCheck);

        while (!facesToCheck.empty()) {
            const auto currentF = OpenMesh::make_smart(mesh.face_handle(facesToCheck.front()), &mesh);
            auto currentHE = currentF.halfedge();

            if (mesh.data(currentF).quadFaceData.patchId == -1) {
                for (int i = 0; i < 4; i++) {
                    const auto neighborF = currentHE.opp().face();

                    if ((tracedEdges.find(currentHE.edge().idx()) == tracedEdges.end())
                        && (mesh.data(neighborF).quadFaceData.patchId == -1)) {

                        facesToCheck.push(neighborF.idx());
                    }

                    currentHE = currentHE.next();
                }

                mesh.data(currentF).quadFaceData.patchId = currentPatch;
            }

            facesToCheck.pop();
        }

        // Skip faces that was already traced
        do {
            faceToStartCheck++;
        } while (mesh.data(startF).quadFaceData.patchId == -1);

        currentPatch++;
    }
}

std::unordered_set<int> skbar::PatchTracer::GetEdges(const skbar::OPQuadMesh::QuadMeshImpl &mesh,
                                                     const std::vector<int> &singularities) {

    std::unordered_set<int> result;

    for (const auto singularity : singularities) {

        const auto vertex = OpenMesh::SmartVertexHandle(singularity, &mesh);

        for (const auto startHE : vertex.outgoing_halfedges()) {

            // Skip if the edge was already traced
            if (result.find(startHE.edge().idx()) == result.end()) {
                auto currentHE = startHE;

                while (!BFIND(singularities, currentHE.to().idx())) {
                    result.insert(currentHE.edge().idx());

                    currentHE = currentHE.next().opp().next();
                }

                // Don't forget to add the last one
                result.insert(currentHE.edge().idx());
            }
        }
    }

    return result;
}

std::vector<int> skbar::PatchTracer::FindSingularities(const skbar::OPQuadMesh::QuadMeshImpl &mesh, bool closed) {

    // This vector is ordered by indexes, because of the iteration that will always place ordered
    std::vector<int> singularities;

    for (const auto it : mesh.vertices()) {

        const auto i = it.idx();

        if (closed) {

            if (mesh.valence(OpenMesh::VertexHandle(i)) != 4) {
                singularities.push_back(i);
            }
        } else {

            if (mesh.valence(OpenMesh::VertexHandle(i)) < 3
                || mesh.valence(OpenMesh::VertexHandle(i)) > 4) {

                singularities.push_back(i);
            }
        }
    }

    return singularities;
}

std::vector<std::vector<int>> skbar::PatchTracer::GetGrid(const skbar::OPQuadMesh &baseMesh, int patch) {

    std::vector<std::vector<int>> grid;

    auto mesh = baseMesh.Get();

    // TODO Add patch tracing

    // TODO Change parameter "closed"
    const auto singularities = FindSingularities(mesh, true);

    const auto v = *singularities.begin();

    // TODO Set to half-edge inside the patch
    const auto startHE = mesh.halfedge_handle(OpenMesh::SmartVertexHandle(v));

    auto firstInLineHE = startHE;

    bool flipFirst = (firstInLineHE.face().idx() == -1);

    do {
        const auto gridLine = FindLine(firstInLineHE, singularities);

        grid.push_back(gridLine);

        // Check if reached another singular vertex
        if ((firstInLineHE.from().idx() != startHE.from().idx())
            && (std::binary_search(singularities.begin(),
                                   singularities.end(), firstInLineHE.from().idx()))) {
            break;
        }

        auto p0 = mesh.point(OpenMesh::VertexHandle(firstInLineHE.from().idx()));
        auto p1 = mesh.point(OpenMesh::VertexHandle(firstInLineHE.to().idx()));

//        Log("(%lf, %lf, %lf) -> (%lf, %lf, %lf)", p0[0], p0[1], p0[2], p1[0], p1[1], p1[2]);

        if (flipFirst) {
            firstInLineHE = firstInLineHE.opp().next().next();
        } else {
            firstInLineHE = firstInLineHE.next().next().opp();
        }
    } while (true);

    return grid;
}

std::vector<int> skbar::PatchTracer::FindLine(const OpenMesh::SmartHalfedgeHandle &handle,
                                              const std::vector<int> &singularities) {
    std::vector<int> line;

    auto current = handle;

    line.push_back(current.from().idx());

    auto samePatch = true;

    do {

        line.push_back(current.to().idx());

        // TODO Check if in another patch
        if (std::binary_search(singularities.begin(), singularities.end(), current.to().idx())) {
            break;
        }

        if (current.face().idx() != -1) {
            if (current.next().opp().face().idx() == -1) {
                samePatch = false;
            }

            current = current.next().opp().next();
        } else {
            current = current.next();
        }

    } while (samePatch);

    return line;
}

