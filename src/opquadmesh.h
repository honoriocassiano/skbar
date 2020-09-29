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

    std::optional<std::vector<std::vector<int>>> GetPatch(int patch) const override;

    void FindPatches() override;

    OPQuadMesh GetTriangulated() const;

    std::size_t GetNumPatches() const override;

    std::map<int, std::vector<std::vector<int>>> &GetPatchFacesMap() {
        return patchFacesMap;
    }

    const std::map<int, std::vector<std::vector<int>>> &GetPatchFacesMap() const {
        return patchFacesMap;
    }

    void ReplacePatch(size_t patchId, const QuadMesh &patch) override;

    BBox GetBBox() const override;

    const QuadMeshImpl &Get() const { return mesh; }

    QuadMeshImpl &Get() { return mesh; }

    std::optional<skbar::Vec2f> GetParametricPosition(int vId, int patch) const override;

private:
    OPQuadMesh(const QuadMeshImpl &mesh);

private:
    QuadMeshImpl mesh;

    std::map<int, std::vector<std::vector<int>>> patchFacesMap;
};

}

#endif /* OPQUADMESH_H */
