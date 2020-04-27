//
// Created by cassiano on 26/04/2020.
//

#include "patchtracer.h"

#include <algorithm>
#include <vector>

#include "utils/debug.h"

namespace skbar {

    void PatchTracer::Trace(QuadMesh &mesh) {

        std::set<std::pair<int, int>> edges;

        auto singularities = FindSingularities(mesh);

        for (const auto &v: singularities) {
            auto start = mesh.halfedge_handle(OpenMesh::SmartVertexHandle(v));

            for (const auto &it : start.from().outgoing_halfedges()) {
                auto current = it;

                do {

                    auto pair = std::make_pair(std::min(current.from().idx(), current.to().idx()),
                                               std::max(current.from().idx(), current.to().idx()));

                    if (edges.find(pair) == edges.end()) {
                        edges.insert(pair);
                    } else {
                        // If already visited, skip
                        break;
                    }

                    auto p0 = mesh.point(OpenMesh::VertexHandle(pair.first));
                    auto p1 = mesh.point(OpenMesh::VertexHandle(pair.second));

//                    Log("(%lf, %lf, %lf) -> (%lf, %lf, %lf)", p0[0], p0[1], p0[2], p1[0], p1[1], p1[2]);

                    if (singularities.find(current.to().idx()) != singularities.end()) {
                        break;
                    }

                    // Just seek the border
                    if (current.face().idx() == -1) {
                        current = current.next();
                    } else {
                        current = current.next().opp().next();
                    }
                } while (true);
            }
        }
    }

    std::set<int> PatchTracer::FindSingularities(const QuadMesh &mesh, bool closed) {

        std::set<int> singularities;

        for (auto it = mesh.vertices().begin(); it != mesh.vertices().end(); it++) {

            int i = std::distance(mesh.vertices().begin(), it);

            if (closed) {

                if (mesh.valence(OpenMesh::VertexHandle(i)) != 4) {
                    singularities.insert(i);
                }
            } else {

                if (mesh.valence(OpenMesh::VertexHandle(i)) < 3
                    || mesh.valence(OpenMesh::VertexHandle(i)) > 4) {

                    singularities.insert(i);
                }
            }


        }

//        printf("%d\n", j);
        return singularities;
    }
}