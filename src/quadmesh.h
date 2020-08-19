#ifndef QUADMESH_H
#define QUADMESH_H

#include "commontypes.h"

#include <string>

namespace skbar {

class QuadMesh {

public:
    QuadMesh() = default;
    virtual ~QuadMesh() = default;

    virtual bool Load(const std::string &filename) = 0;

    virtual void FindPatches() = 0;

    virtual std::size_t GetNumPatches() const = 0;

    virtual QuadMesh *GetPatch(std::size_t patchId) = 0;

    virtual void ReplacePatch(size_t patchId, const QuadMesh &patch) = 0;

    virtual void Render() const = 0;

    virtual BBox GetBBox() const = 0;
};
}

#endif /* QUADMESH_H */
