#include "gldrawer.h"
#include "drawereventprocessor.h"

skbar::GLDrawer::GLDrawer(Viewer *_viewer) : viewer(_viewer) {

    processor = new DrawerEventProcessor(this);
}

skbar::GLDrawer::~GLDrawer() {
	delete processor;

	processor = nullptr;
}

void skbar::GLDrawer::Reset() {
	GetMesh().GetSketch().Reset();
}

bool skbar::GLDrawer::ProcessEvent(const skbar::Event &event) {
	return processor->Process(event);
}

skbar::Intersection<int, skbar::Vec3f> skbar::GLDrawer::GetClosestIntersection(
	const skbar::Line<skbar::Vec3f> &ray) {

	return viewer->GetMesh().GetClosestIntersection(ray);
}
