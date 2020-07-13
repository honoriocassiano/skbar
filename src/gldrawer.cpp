#include "gldrawer.h"
#include "drawereventprocessor.h"

skbar::GLDrawer::GLDrawer(Viewer *_viewer) : viewer(_viewer) {

    processor = new DrawerEventProcessor(this);
}

skbar::GLDrawer::~GLDrawer() {

}

bool skbar::GLDrawer::Draw(const Vec3f &origin, const Vec3f &direction) {

}

// skbar::Line<skbar::Vec3f> skbar::GLDrawer::GetLine() const {
//     auto camera = dynamic_cast<const GLCamera &>(viewer->GetCamera());
//
//     camera.
// }
//
