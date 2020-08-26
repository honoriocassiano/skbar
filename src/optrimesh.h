#ifndef OPTRIMESH_H
#define OPTRIMESH_H

#include "trimesh.h"
#include "meshtypes.h"

namespace skbar {

class OPTriMesh : public TriMesh {

public:
    typedef OpenMesh::TriMesh_ArrayKernelT<TriMeshTraits> TriMeshImpl;

    OPTriMesh() = default;

    virtual ~OPTriMesh() = default;

    bool Load(const QuadMesh &mesh) override;

    [[nodiscard]] int GetQuadFaceId(int triangleId) const override;

    bool AddPointOnIntersection(const Intersection<int, Vec3f> &intersection) override;

    std::tuple<bool, std::vector<Intersection<int, Vec3f>>> GetIntersectionsBetween(
        const Intersection<int, Vec3f> &p1, const Intersection<int, Vec3f> &p2,
        const skbar::Line<Vec3f> &viewDirection, const Projection<float> &projection) const override;

    Intersection<int, Vec3f> GetClosestIntersection(const Line<Vec3f> &ray) const override;

    TriMeshImpl &Get() { return mesh; }

    const TriMeshImpl &Get() const { return mesh; }
private:

    TriMeshImpl mesh;

};

}

#endif /* OPTRIMESH_H */
