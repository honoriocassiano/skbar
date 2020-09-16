#ifndef OPQUADMESH_H
#define OPQUADMESH_H

#include "quadmesh.h"
#include "meshtypes.h"

namespace skbar {

class OPQuadMesh : public QuadMesh {

public:
    typedef OpenMesh::PolyMesh_ArrayKernelT<QuadMeshTraits> QuadMeshImpl;

    OPQuadMesh() = default;

    virtual ~OPQuadMesh() = default;

    bool Load(const std::string &filename) override;

    bool Save(const std::string &filename) const override;

    void FindPatches() override;

    OPQuadMesh GetTriangulated() const;

    std::size_t GetNumPatches() const override;

    QuadMesh *GetPatch(std::size_t patchId) override;

    void ReplacePatch(size_t patchId, const QuadMesh &patch) override;

    BBox GetBBox() const override;

    const QuadMeshImpl &Get() const { return mesh; }

    QuadMeshImpl &Get() { return mesh; }

private:
    OPQuadMesh(const QuadMeshImpl &mesh);

private:
    QuadMeshImpl mesh;
};

}

#endif /* OPQUADMESH_H */
