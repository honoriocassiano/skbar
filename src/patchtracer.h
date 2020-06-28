//
// Created by cassiano on 26/04/2020.
//

#ifndef SKBAR_PATCHTRACER_H
#define SKBAR_PATCHTRACER_H

#include "meshtypes.h"

#include <set>
#include <vector>

namespace skbar {

    class PatchTracer {

    private:
        static std::set<int> FindSingularities(const QuadMesh &mesh, bool closed = true);

        static std::vector<int> FindLine(const OpenMesh::SmartHalfedgeHandle& handle, const std::set<int>& singularities) ;

    public:
        static void Trace(QuadMesh &mesh);

        static std::vector<std::vector<int>> GetGrid(const QuadMesh& mesh, int patch);
    };
}

#endif //SKBAR_PATCHTRACER_H
