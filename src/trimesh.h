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

    // TODO REMOVE THIS 'INT' AND ADD AS TEMPLATE
    virtual bool AddPointOnIntersection(const Intersection<int, Vec3f> &ray) = 0;

    virtual bool CheckIntersection(const Line<Vec3f> &ray) const = 0;

    virtual Intersection<int, Vec3f> GetClosestIntersection(const Line<Vec3f> &ray) const = 0;

    // TODO Add methods to manipulate curves
};
}

#endif /* TRIMESH_H */
