#include "opmeshrenderer.h"

#include <GL/gl.h>

#include "utils/debug.h"

skbar::OPMeshRenderer::OPMeshRenderer() {

}

skbar::OPMeshRenderer::~OPMeshRenderer() {

}

void skbar::OPMeshRenderer::Render(const skbar::OPTriMesh::TriMeshImpl &mesh, const skbar::RenderOptions &options) {

	glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);

	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(10.0, 10.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBegin(GL_TRIANGLES);

    glColor3f(1, 1, 1);

    for (auto it : mesh.faces()) {

        auto normal = mesh.normal(it);

        glNormal3f(normal[0], normal[1], normal[2]);

        for (auto vit : it.vertices()) {
            auto pos = mesh.point(vit);

            glVertex3f(pos[0], pos[1], pos[2]);
        }
    }

    glEnd();

	glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
}

void skbar::OPMeshRenderer::RenderSketch(const skbar::OPSketch &sketch, bool closed, const skbar::SketchRenderOptions &options) {

    if (sketch.Size() > 0) {

        glDisable(GL_LIGHTING);
        glDepthFunc(GL_ALWAYS);

        glColor3fv(options.lineColor);
        glLineWidth(options.lineWidth);

        glBegin(closed ? GL_LINE_LOOP : GL_LINE_STRIP);

        for (std::size_t i = 0; i < sketch.Size(); i++) {
            auto point = sketch.Data()[i].Position();

            glVertex3f(point[0], point[1], point[2]);
        }

        glEnd();

        glDepthFunc(GL_LESS);
        glEnable(GL_LIGHTING);
    }
}