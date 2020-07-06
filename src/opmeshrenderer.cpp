#include "opmeshrenderer.h"

#include <GL/gl.h>

#include "utils/debug.h"

skbar::OPMeshRenderer::OPMeshRenderer() {

}

skbar::OPMeshRenderer::~OPMeshRenderer() {

}

void skbar::OPMeshRenderer::Render(const skbar::OPTriMesh::TriMeshImpl &mesh, const skbar::RenderOptions &options) {

	glEnable(GL_LIGHTING);
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
	glDisable(GL_LIGHTING);
}

