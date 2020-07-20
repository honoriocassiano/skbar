#ifndef OPMESHRENDERER_H
#define OPMESHRENDERER_H

#include "optrimesh.h"
#include "opquadmesh.h"
#include "opsketch.h"

namespace skbar {

struct RenderOptions {

    RenderOptions() {}
    virtual ~RenderOptions() {}

};

struct SketchRenderOptions {

	float lineColor[3];
	float lineWidth;

    SketchRenderOptions() : lineColor{0, 1, 0}, lineWidth(2.5) {}
    virtual ~SketchRenderOptions() {}

};

class OPMeshRenderer {

public:

    OPMeshRenderer();

    virtual ~OPMeshRenderer();

    static void Render(const OPTriMesh::TriMeshImpl &mesh, const RenderOptions &options = RenderOptions());

    static void RenderSketch(const OPSketch &sketch, bool closed, const SketchRenderOptions &options = SketchRenderOptions());

};

}

#endif /* OPMESHRENDERER_H */
