#include "opsketch.h"

#include "optrimesh.h"
#include "opmeshrenderer.h"

#include "utils/debug.h"

skbar::OPSketch::OPSketch(EditableMesh *_mesh) : mesh(_mesh),
    started(false) {

}

skbar::OPSketch::~OPSketch() {

}

bool skbar::OPSketch::Start() {
    started = true;

    // TODO Reset sketch here
}
bool skbar::OPSketch::Close() {
    started = false;
}

void skbar::OPSketch::Reset() {
    data.clear();
}

bool skbar::OPSketch::AddPoint(const skbar::Line<skbar::Vec3f> &ray, const skbar::Intersection<int, skbar::Vec3f> &intersection) {

    bool added = false;

    // Log("Face after: %d", intersection.Face());

    if (started) {

        // auto trimesh = dynamic_cast<OPTriMesh &>(mesh->GetTri());

        #warning "TODO Uncomment this"
        // added = trimesh.AddPointOnIntersection(intersection);

        #warning "TODO Chance this face type"
        data.emplace_back(intersection.Face(), intersection.Position(), SketchVertex::EType::FACE);

        added = true;
    }

    return added;
}

bool skbar::OPSketch::IsStarted() const {
    return started;
}

void skbar::OPSketch::Render() const {
    OPMeshRenderer::RenderSketch(*this, !started);
}