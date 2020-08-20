#include "opmeshrenderer.h"

#include "utils/debug.h"

skbar::Vec3f skbar::renderer::OPMeshRenderer::GetColor(int patch) {
    const static skbar::Vec3f colors[60]{{0.00, 0.00, 0.25},
                                         {0.00, 0.00, 0.50},
                                         {0.00, 0.00, 0.75},
                                         {0.00, 0.25, 0.00},
                                         {0.00, 0.25, 0.25},
                                         {0.00, 0.25, 0.50},
                                         {0.00, 0.25, 0.75},
                                         {0.00, 0.50, 0.00},
                                         {0.00, 0.50, 0.25},
                                         {0.00, 0.50, 0.50},
                                         {0.00, 0.50, 0.75},
                                         {0.00, 0.75, 0.00},
                                         {0.00, 0.75, 0.25},
                                         {0.00, 0.75, 0.50},
                                         {0.00, 0.75, 0.75},
                                         {0.25, 0.00, 0.00},
                                         {0.25, 0.00, 0.25},
                                         {0.25, 0.00, 0.50},
                                         {0.25, 0.00, 0.75},
                                         {0.25, 0.25, 0.00},
                                         {0.25, 0.25, 0.50},
                                         {0.25, 0.25, 0.75},
                                         {0.25, 0.50, 0.00},
                                         {0.25, 0.50, 0.25},
                                         {0.25, 0.50, 0.50},
                                         {0.25, 0.50, 0.75},
                                         {0.25, 0.75, 0.00},
                                         {0.25, 0.75, 0.25},
                                         {0.25, 0.75, 0.50},
                                         {0.25, 0.75, 0.75},
                                         {0.50, 0.00, 0.00},
                                         {0.50, 0.00, 0.25},
                                         {0.50, 0.00, 0.50},
                                         {0.50, 0.00, 0.75},
                                         {0.50, 0.25, 0.00},
                                         {0.50, 0.25, 0.25},
                                         {0.50, 0.25, 0.50},
                                         {0.50, 0.25, 0.75},
                                         {0.50, 0.50, 0.00},
                                         {0.50, 0.50, 0.25},
                                         {0.50, 0.50, 0.75},
                                         {0.50, 0.75, 0.00},
                                         {0.50, 0.75, 0.25},
                                         {0.50, 0.75, 0.50},
                                         {0.50, 0.75, 0.75},
                                         {0.75, 0.00, 0.00},
                                         {0.75, 0.00, 0.25},
                                         {0.75, 0.00, 0.50},
                                         {0.75, 0.00, 0.75},
                                         {0.75, 0.25, 0.00},
                                         {0.75, 0.25, 0.25},
                                         {0.75, 0.25, 0.50},
                                         {0.75, 0.25, 0.75},
                                         {0.75, 0.50, 0.00},
                                         {0.75, 0.50, 0.25},
                                         {0.75, 0.50, 0.50},
                                         {0.75, 0.50, 0.75},
                                         {0.75, 0.75, 0.00},
                                         {0.75, 0.75, 0.25},
                                         {0.75, 0.75, 0.50}};

    if (patch < 0) {
        return skbar::Vec3f{1, 1, 1};
    } else {
        return colors[patch % 60];
    }
}

void skbar::renderer::OPMeshRenderer::Render(const skbar::EditableMesh &mesh, const skbar::renderer::Options &options) {
    if (options.mesh == Options::Mesh::QUAD) {
        RenderQuad(mesh, options);
    } else if (options.mesh == Options::Mesh::TRI) {
        Render(dynamic_cast<const OPTriMesh &>(mesh.GetTri()).Get(), options);
    }
}

void skbar::renderer::OPMeshRenderer::Render(const skbar::OPTriMesh::TriMeshImpl &mesh, const skbar::renderer::Options &options) {

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_OFFSET_FILL);

    glEnable(GL_LIGHTING);

    glPolygonOffset(10.0, 10.0);

    if (options.triOptions.drawEdges) {
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

void skbar::renderer::OPMeshRenderer::RenderSketch(const skbar::OPSketch &sketch, bool closed,
                                                   const skbar::renderer::Options &options) {

    if (sketch.Size() > 0) {

        glDisable(GL_LIGHTING);
        glDepthFunc(GL_ALWAYS);

        glColor3fv(options.skOptions.lineColor);
        glLineWidth(options.skOptions.lineWidth);

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

void skbar::renderer::OPMeshRenderer::RenderPatches(const skbar::OPQuadMesh::QuadMeshImpl &mesh) {
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(0.1f);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glColor3f(2.0f, 2.0f, 2.0f);

    glBegin(GL_LINES);

    for (auto edge : mesh.edges()) {

        if (mesh.data(edge).quadEdgeData.patchEdge) {
            auto point1 = mesh.point(edge.v0());
            auto point2 = mesh.point(edge.v1());

            glVertex3f(point1[0], point1[1], point1[2]);
            glVertex3f(point2[0], point2[1], point2[2]);
        }
    }

    glEnd();

    glDisable(GL_LINE_SMOOTH);
}


void
skbar::renderer::OPMeshRenderer::RenderQuad(const skbar::EditableMesh &eMesh,
                                            const skbar::renderer::Options &options) {

    const auto &mesh = dynamic_cast<const OPQuadMesh &>(eMesh.GetQuad()).Get();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_OFFSET_FILL);

    glPolygonOffset(10.0, 10.0);

    if (options.quadOptions.drawEdges) {
        RenderEdges(mesh);
    }

    if (options.quadOptions.drawPatches) {
        RenderPatches(mesh);
    }

    if (options.drawSketch) {
        const auto &opSketch = dynamic_cast<const OPSketch &>(eMesh.GetSketch());

        RenderSketch(opSketch, !opSketch.IsStarted());
    }

    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);

    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glBegin(GL_QUADS);

    if (!options.quadOptions.drawPatches) {
        glColor3f(1, 1, 1);
    }

    for (auto it : mesh.faces()) {

        auto normal = mesh.normal(it);

        if (options.quadOptions.drawPatches) {
            const auto color = GetColor(mesh.data(it).quadFaceData.patchId);

            glColor3f(color[0], color[1], color[2]);
        }

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
