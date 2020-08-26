#ifndef OPMESHRENDERER_H
#define OPMESHRENDERER_H

#include "optrimesh.h"
#include "opquadmesh.h"
#include "opsketch.h"
#include "renderoptions.h"
#include "meshrenderer.h"

#include <GL/glew.h>
#include <GL/gl.h>

namespace skbar::renderer {

class OPMeshRenderer : public MeshRenderer {

public:

    OPMeshRenderer() = default;

    virtual ~OPMeshRenderer() = default;

    void Render(const EditableMesh &mesh, const Options &options) override;

private:
    void RenderQuad(const EditableMesh &mesh, const Options &options = Options());

    void Render(const OPTriMesh::TriMeshImpl &mesh, const Options &options = Options());

    void RenderPatches(const OPQuadMesh::QuadMeshImpl &mesh);

    void RenderSketch(const OPSketch &sketch, bool closed, const Options &options = Options());

    skbar::Vec3f GetColor(int patch);

    void RenderEdges(const OPTriMesh::TriMeshImpl &mesh);

    void RenderEdges(const OPQuadMesh::QuadMeshImpl &mesh);
};
}

#endif /* OPMESHRENDERER_H */
