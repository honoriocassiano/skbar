#ifndef OPMESHRENDERER_H
#define OPMESHRENDERER_H

#include "optrimesh.h"
#include "opquadmesh.h"

namespace skbar {

struct RenderOptions {

    RenderOptions() {}
    virtual ~RenderOptions() {}

};

class OPMeshRenderer {

public:

    OPMeshRenderer();
    virtual ~OPMeshRenderer();

    static void Render(const OPTriMesh::TriMeshImpl &mesh, const RenderOptions &options = RenderOptions());

};

}

#endif /* OPMESHRENDERER_H */
