//
// Created by cassiano on 26/04/2020.
//

#include "patchtracer.h"

#include <algorithm>

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

    std::vector<std::vector<int>> PatchTracer::GetGrid(const QuadMesh &mesh, int patch) {
        std::vector<std::vector<int>> grid;

        // TODO Add patch tracing

        // TODO Change parameter "closed"
        auto singularities = FindSingularities(mesh, false);

        auto v = *singularities.begin();

        // TODO Set to half-edge inside the patch
        auto start = mesh.halfedge_handle(OpenMesh::SmartVertexHandle(v));

        auto firstInLine = start;

        bool flipFirst = (firstInLine.face().idx() == -1);

        do {
            grid.push_back(FindLine(firstInLine, singularities));

            if ((firstInLine.from().idx() != start.from().idx())
                && (singularities.find(firstInLine.from().idx()) != singularities.end())) {
                break;
            }

            auto p0 = mesh.point(OpenMesh::VertexHandle(firstInLine.from().idx()));
            auto p1 = mesh.point(OpenMesh::VertexHandle(firstInLine.to().idx()));

            Log("(%lf, %lf, %lf) -> (%lf, %lf, %lf)", p0[0], p0[1], p0[2], p1[0], p1[1], p1[2]);

            if (flipFirst) {
                firstInLine = firstInLine.opp().next().next();
            } else {
                firstInLine = firstInLine.next().next().opp();
            }
        } while (true);

        return grid;
    }

    std::vector<int> PatchTracer::FindLine(const OpenMesh::SmartHalfedgeHandle &handle,
                                           const std::set<int> &singularities) {
        std::vector<int> line;

        auto current = handle;

        line.push_back(current.from().idx());

        auto samePatch = true;

        do {

            line.push_back(current.to().idx());

            // TODO Check if in another patch
            if (singularities.find(current.to().idx()) != singularities.end()) {
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
}