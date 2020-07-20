#ifndef OPTRIMESH_H
#define OPTRIMESH_H

#include "trimesh.h"
#include "meshtypes.h"

namespace skbar {

class OPTriMesh : public TriMesh {

public:
    typedef OpenMesh::TriMesh_ArrayKernelT<TriMeshTraits> TriMeshImpl;

    OPTriMesh();

    virtual ~OPTriMesh();

    bool Load(const QuadMesh &mesh) override;

    std::size_t GetQuadFaceId(std::size_t triangleId) const override;

    bool AddPointOnIntersection(const Intersection<int, Vec3f> &intersection) override;

    Intersection<int, Vec3f> GetClosestIntersection(const Line<Vec3f> &ray) const override;

    // TODO Add to base class?
    bool CheckIntersection(const Line<Vec3f> &ray) const override;

    void Render() const override;

    TriMeshImpl &Get() { return mesh; }

    const TriMeshImpl &Get() const { return mesh; }
private:

    TriMeshImpl mesh;

};

}

#endif /* OPTRIMESH_H */
