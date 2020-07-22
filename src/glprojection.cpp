#include "glprojection.h"

#include <GL/glew.h>
#include <GL/gl.h>

skbar::GLProjection::GLProjection() {
	#warning "TODO Initialize with identity"
}

skbar::GLProjection::GLProjection(const std::array<double, 16> &_modelview, const std::array<double, 16> &_projection,
		const std::array<int, 4> &_viewport) {

	modelview = _modelview;
	projection = _projection;
	viewport = _viewport;
}

skbar::GLProjection::~GLProjection() {

}

skbar::Vec2f skbar::GLProjection::Project(const skbar::Vec3f &worldPoint) const {

	double x, y, z;

	gluProject(worldPoint[0], worldPoint[1], worldPoint[2], modelview.data(), projection.data(), viewport.data(), &x, &y, &z);

	#warning "TODO Check if need more calculations"

	return Vec2f{x, y};

}

skbar::Vec3f skbar::GLProjection::UnProject(const skbar::Vec2f &windowPoint) const {

    double x, y, z;
	gluUnProject(windowPoint[0], windowPoint[1], 1, modelview.data(), projection.data(), viewport.data(), &x, &y, &z);

	return Vec3f{x, y, z};
}
