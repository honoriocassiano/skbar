#include "opsketch.h"

#include "optrimesh.h"

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

bool skbar::OPSketch::AddPoint(const skbar::Line<skbar::Vec3f> &ray, const skbar::Intersection<int, skbar::Vec3f> &intersection) {

	bool added = false;

	if (started) {

	    auto trimesh = dynamic_cast<OPTriMesh &>(mesh->GetTri());

	    added = trimesh.AddPointOnIntersection(intersection);
    }

    return added;
}

bool skbar::OPSketch::IsStarted() const {
	return started;
}