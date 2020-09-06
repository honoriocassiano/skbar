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

    unsigned int currentPatch = 0;

    unsigned int faceToStartCheck = 0;

    while (faceToStartCheck < mesh.n_faces()) {

        const auto startF = mesh.face_handle(faceToStartCheck);

        // Compute only faces not visited yet
        if (mesh.data(startF).quadFaceData.patchId == -1) {
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

                    mesh.data(currentF).quadFaceData.patchId = int(currentPatch);
                }

                facesToCheck.pop();
            }

            currentPatch++;
        }

        // Skip faces that was already traced
        do {
            faceToStartCheck++;
        } while (faceToStartCheck < mesh.n_faces() &&
                 (mesh.data(mesh.face_handle(faceToStartCheck)).quadFaceData.patchId != -1));
    }

    TraceGrid(mesh, currentPatch);
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

void skbar::PatchTracer::TraceGrid(skbar::OPQuadMesh::QuadMeshImpl &mesh,
                                   const unsigned int numPatches) {

    std::unordered_set<int> tracedPatches;

    std::set<int> cornerVertices;

    for (const auto vertex : mesh.vertices()) {

        std::unordered_set<int> delimitedPatches;

        for (const auto he : vertex.outgoing_halfedges()) {

            const auto patchId = mesh.data(he.face()).quadFaceData.patchId;

            delimitedPatches.insert(patchId);
        }

        if (delimitedPatches.size() == mesh.valence(vertex)) {
            cornerVertices.insert(vertex.idx());
        }
    }

    for (const auto vertexId : cornerVertices) {

        if (tracedPatches.size() == numPatches) {
            // Stop if all patches was already traced
            break;
        }

        const auto vertex = OpenMesh::SmartVertexHandle(vertexId, &mesh);

        for (const auto firstHE : vertex.outgoing_halfedges()) {

            const int currentPatch = mesh.data(firstHE.face()).quadFaceData.patchId;

            if (tracedPatches.find(currentPatch) == tracedPatches.end()) {
                auto currentHE = firstHE;
                auto firstInLineHE = currentHE;

                // Start with minimum vertices grid size
                int gridWidth = 2;
                int gridHeight = 2;

                // Find vertex grid dimensions
                {
                    // Don't count the first and the last vertices
                    while (cornerVertices.find(currentHE.to().idx()) == cornerVertices.end()) {
                        gridWidth++;

                        currentHE = currentHE.next().opp().next();
                    }

                    // Rotate to find the last columns
                    currentHE = currentHE.next();

                    // Don't count the first and the last vertices
                    while (cornerVertices.find(currentHE.to().idx()) == cornerVertices.end()) {
                        gridHeight++;

                        currentHE = currentHE.next().opp().next();
                    }
                }

                int currentLine;

                for (currentLine = 0; currentLine < gridHeight - 1; currentLine++) {

                    int currentColumn;

                    currentHE = firstInLineHE;

                    for (currentColumn = 0; currentColumn < gridWidth - 1; currentColumn++) {

                        const Vec2f coordinate{float(currentLine) / float(gridHeight - 1),
                                               float(currentColumn) / float(gridWidth - 1)};

                        mesh.data(currentHE.from()).quadVertexData.patchParametrizations[currentPatch] =
                                coordinate;

                        currentHE = currentHE.next().opp().next();
                    }

                    // Don't forget to add the last one
                    mesh.data(currentHE.to()).quadVertexData.patchParametrizations[currentPatch] =
                            Vec2f{float(currentLine) / float(gridHeight - 1),
                                  float(currentColumn) / float(gridWidth - 1)};

                    firstInLineHE = firstInLineHE.next().next().opp();
                }

                currentHE = firstInLineHE;

                // Add the last vertices line
                {
                    int currentColumn;

                    currentHE = firstInLineHE;

                    for (currentColumn = 0; currentColumn < gridWidth - 1; currentColumn++) {

                        const Vec2f coordinate{float(currentLine) / float(gridHeight - 1),
                                               float(currentColumn) / float(gridWidth - 1)};

                        mesh.data(currentHE.from()).quadVertexData.patchParametrizations[currentPatch] =
                                coordinate;

                        currentHE = currentHE.next().opp().next();
                    }

                    // Don't forget to add the last one
                    mesh.data(currentHE.to()).quadVertexData.patchParametrizations[currentPatch] =
                            Vec2f{float(currentLine) / float(gridHeight - 1),
                                  float(currentColumn) / float(gridWidth - 1)};
                }

                tracedPatches.insert(currentPatch);
            }
        }
    }
}

