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

private:

    TriMeshImpl mesh;

};

}

#endif /* OPTRIMESH_H */
