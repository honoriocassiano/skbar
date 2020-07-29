#include "opmeshrenderer.h"

#include "utils/debug.h"

void skbar::renderer::Render(const skbar::OPTriMesh::TriMeshImpl &mesh, const skbar::renderer::MeshOptions &options) {

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_OFFSET_FILL);

    glEnable(GL_LIGHTING);

    glPolygonOffset(10.0, 10.0);

    if (options.drawEdges) {
        RenderEdges(mesh);
    }

	glEnable(GL_COLOR_MATERIAL);

	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
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

    glDisable(GL_LIGHTING);
	glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_DEPTH_TEST);
}

void skbar::renderer::RenderSketch(const skbar::OPSketch &sketch, bool closed, const skbar::renderer::SketchOptions &options) {

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

//void skbar::renderer::RenderEdges(const skbar::OPTriMesh::TriMeshImpl &mesh) {
//
//    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
//    glEnable(GL_LINE_SMOOTH);
//    glLineWidth(0.1f);
//
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//
//    glColor3f(0.3f, 0.3f, 0.3f);
//
//    glBegin(GL_LINES);
//
//    for (auto edge : mesh.edges()) {
//        auto point1 = mesh.point(edge.v0());
//        auto point2 = mesh.point(edge.v1());
//
//        glVertex3f(point1[0], point1[1], point1[2]);
//        glVertex3f(point2[0], point2[1], point2[2]);
//    }
//
//    glEnd();
//
//    glDisable(GL_LINE_SMOOTH);
//}

void skbar::renderer::Render(const skbar::OPQuadMesh::QuadMeshImpl &mesh, const skbar::renderer::MeshOptions &options) {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_OFFSET_FILL);

    glEnable(GL_LIGHTING);

    glPolygonOffset(10.0, 10.0);

    if (options.drawEdges) {
        RenderEdges(mesh);
    }

    glEnable(GL_COLOR_MATERIAL);

    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glBegin(GL_QUADS);

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

    glDisable(GL_LIGHTING);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_DEPTH_TEST);
}
