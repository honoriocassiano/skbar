#include "opsketch.h"

#include "optrimesh.h"

skbar::OPSketch::OPSketch(EditableMesh *_mesh) : mesh(_mesh) {

}

skbar::OPSketch::~OPSketch() {

}

bool skbar::OPSketch::AddPoint(const skbar::Line<skbar::Vec3f> &ray) {
    auto trimesh = dynamic_cast<OPTriMesh &>(mesh->GetTri());

    bool added = trimesh.AddPointOnIntersection(ray);

    return added;
}

