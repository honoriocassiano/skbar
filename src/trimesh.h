#ifndef TRIMESH_H
#define TRIMESH_H

#include "quadmesh.h"

namespace skbar {

class TriMesh {

public:
    TriMesh() {}
    virtual ~TriMesh() {}

    virtual bool Load(const QuadMesh &mesh) = 0;

    virtual std::size_t GetQuadFaceId(std::size_t triangleId) const = 0;

    virtual void Render() const = 0;

    // TODO Add methods to manipulate curves
};
}

#endif /* TRIMESH_H */
