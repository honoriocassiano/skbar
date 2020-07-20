#include "gldrawer.h"
#include "drawereventprocessor.h"

skbar::GLDrawer::GLDrawer(Viewer *_viewer) : viewer(_viewer) {

    processor = new DrawerEventProcessor(this);
}

skbar::GLDrawer::~GLDrawer() {
	delete processor;

	processor = nullptr;
}

bool skbar::GLDrawer::Draw(const Vec3f &origin, const Vec3f &direction) {
	// TODO Implement this
	return false;
}

bool skbar::GLDrawer::ProcessEvent(const skbar::Event &event) {
	return processor->Process(event);
}

bool skbar::GLDrawer::IsStarted() const {
	return GetMesh().GetSketch().IsStarted();
}

skbar::Intersection<int, skbar::Vec3f> skbar::GLDrawer::GetClosestIntersection(
	const skbar::Line<skbar::Vec3f> &ray) {

	return viewer->GetMesh().GetClosestIntersection(ray);
}

// skbar::Line<skbar::Vec3f> skbar::GLDrawer::GetLine() const {
//     auto camera = dynamic_cast<const GLCamera &>(viewer->GetCamera());
//
//     camera.
// }
//
