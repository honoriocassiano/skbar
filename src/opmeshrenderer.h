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

    template<typename OPMesh>
    void RenderEdges(const OPMesh &mesh) {
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
        glEnable(GL_LINE_SMOOTH);
        glLineWidth(2.0f);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glColor3f(0.3f, 0.3f, 0.3f);

        glBegin(GL_LINES);

        for (auto edge : mesh.edges()) {

            auto point1 = mesh.point(edge.v0());
            auto point2 = mesh.point(edge.v1());

            glVertex3f(point1[0], point1[1], point1[2]);
            glVertex3f(point2[0], point2[1], point2[2]);
        }

        glEnd();

        glDisable(GL_LINE_SMOOTH);
    }
};
}

#endif /* OPMESHRENDERER_H */
