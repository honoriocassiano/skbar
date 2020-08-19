//
// Created by cassiano on 26/04/2020.
//

#include "patchtracer.h"

#include <algorithm>

#include "utils/debug.h"

void skbar::PatchTracer::Trace(skbar::OPQuadMesh &baseMesh) {

    auto mesh = baseMesh.Get();

    std::set<int> edges;

    auto singularities = FindSingularities(mesh);

    for (const auto &v: singularities) {
        const auto startHE = mesh.halfedge_handle(OpenMesh::SmartVertexHandle(v));

        for (const auto itHE : startHE.from().outgoing_halfedges()) {
            auto currentHE = itHE;

            do {

//                auto pair = std::make_pair(std::min(currentHE.from().idx(), currentHE.to().idx()),
//                                           std::max(currentHE.from().idx(), currentHE.to().idx()));
                    const auto edge = currentHE.edge().idx();

                if (edges.find(edge) == edges.end()) {
                    edges.insert(edge);
                } else {
                    // If already visited, skip
                    break;
                }

                // auto p0 = mesh.point(OpenMesh::VertexHandle(pair.first));
                // auto p1 = mesh.point(OpenMesh::VertexHandle(pair.second));

//                    Log("(%lf, %lf, %lf) -> (%lf, %lf, %lf)", p0[0], p0[1], p0[2], p1[0], p1[1], p1[2]);

                if (std::binary_search(singularities.begin(), singularities.end(), currentHE.to().idx())) {
                    break;
                }

                // Just seek the border
                if (currentHE.face().idx() == -1) {
                    currentHE = currentHE.next();
                } else {
                    currentHE = currentHE.next().opp().next();
                }
            } while (true);
        }
    }
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
    const auto singularities = FindSingularities(mesh, false);

    const auto v = *singularities.begin();

    // TODO Set to half-edge inside the patch
    const auto startHE = mesh.halfedge_handle(OpenMesh::SmartVertexHandle(v));

    auto firstInLineHE = startHE;

    bool flipFirst = (firstInLineHE.face().idx() == -1);

    do {
        grid.push_back(FindLine(firstInLineHE, singularities));

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

