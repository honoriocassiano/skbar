//
// Created by cassiano on 26/04/2020.
//

#ifndef SKBAR_PATCHTRACER_H
#define SKBAR_PATCHTRACER_H

#include "opquadmesh.h"

#include <unordered_set>
#include <vector>

namespace skbar {

class PatchTracer {

private:
    /**
     * Returns all singularities ordered by vertex id
     */
    static std::vector<int> FindSingularities(const OPQuadMesh::QuadMeshImpl &mesh, bool closed = true);

    static std::unordered_set<int> GetEdges(const OPQuadMesh::QuadMeshImpl &mesh, const std::vector<int> &singularities);

    static std::vector<int> FindLine(const OpenMesh::SmartHalfedgeHandle& handle, const std::vector<int>& singularities);

    static void TraceGrid(OPQuadMesh &mesh, unsigned int numPatches);

public:
    static void Trace(OPQuadMesh &mesh);
};
}

#endif //SKBAR_PATCHTRACER_H
