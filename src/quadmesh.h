#ifndef QUADMESH_H
#define QUADMESH_H

#include "commontypes.h"

#include <string>
#include <vector>
#include <optional>

namespace skbar {

class QuadMesh {

public:
    QuadMesh() = default;
    virtual ~QuadMesh() = default;

    virtual bool Load(const std::string &filename) = 0;

    virtual bool Save(const std::string &filename) const = 0;

    virtual std::optional<skbar::Vec2f> GetParametricPosition(int vId, int patch) const = 0;

    virtual std::optional<std::vector<std::vector<int>>> GetPatch(int patch) const = 0;

    virtual void FindPatches() = 0;

    virtual std::size_t GetNumPatches() const = 0;

    virtual void ReplacePatch(size_t patchId, const QuadMesh &patch) = 0;

    virtual BBox GetBBox() const = 0;
};
}

#endif /* QUADMESH_H */
