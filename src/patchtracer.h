//
// Created by cassiano on 26/04/2020.
//

#ifndef SKBAR_PATCHTRACER_H
#define SKBAR_PATCHTRACER_H

#include "opquadmesh.h"

#include <set>
#include <vector>

namespace skbar {

class PatchTracer {

private:
    /**
     * Returns all singularities ordered by vertex id
     */
    static std::vector<int> FindSingularities(const OPQuadMesh::QuadMeshImpl &mesh, bool closed = true);

    static std::vector<int> FindLine(const OpenMesh::SmartHalfedgeHandle& handle, const std::vector<int>& singularities) ;

public:
    static void Trace(OPQuadMesh &mesh);

    static std::vector<std::vector<int>> GetGrid(const OPQuadMesh& mesh, int patch);
};
}

#endif //SKBAR_PATCHTRACER_H
