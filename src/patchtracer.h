//
// Created by cassiano on 26/04/2020.
//

#ifndef SKBAR_PATCHTRACER_H
#define SKBAR_PATCHTRACER_H

#include "meshtypes.h"

#include <set>

namespace skbar {

    class PatchTracer {

    private:
        static std::set<int> FindSingularities(const QuadMesh &mesh, bool closed = true);

    public:
        static void Trace(QuadMesh &mesh);
    };
}

#endif //SKBAR_PATCHTRACER_H
