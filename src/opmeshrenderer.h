#ifndef OPMESHRENDERER_H
#define OPMESHRENDERER_H

#include "optrimesh.h"
#include "opquadmesh.h"
#include "opsketch.h"

#include <GL/gl.h>

namespace skbar {

namespace renderer {

struct MeshOptions {
    bool drawEdges;
};

struct SketchOptions {

	float lineColor[3];
	float lineWidth;

    SketchOptions() : lineColor{0, 1, 0}, lineWidth(2.5) {}
    virtual ~SketchOptions() {}

};

void Render(const OPQuadMesh::QuadMeshImpl &mesh, const MeshOptions &options = MeshOptions());

void Render(const OPTriMesh::TriMeshImpl &mesh, const MeshOptions &options = MeshOptions());

void RenderSketch(const OPSketch &sketch, bool closed, const SketchOptions &options = SketchOptions());

template <typename OPMesh>
void RenderEdges(const OPMesh &mesh) {
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(0.1f);

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
}
}

#endif /* OPMESHRENDERER_H */
