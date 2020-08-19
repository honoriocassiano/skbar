#ifndef GLPROJECTION_H
#define GLPROJECTION_H

#include "projection.h"

#include <array>

namespace skbar {

class GLProjection : public Projection<float> {

public:

	GLProjection();

	GLProjection(const std::array<double, 16> &modelview,
		const std::array<double, 16> &projection,
		const std::array<int, 4> &viewport);

	virtual ~GLProjection();
	
	virtual Vec2f Project(const Vec3f &point) const override;

	virtual Vec3f UnProject(const Vec2f &point) const override;

private:
	std::array<double, 16> modelview;
	std::array<double, 16> projection;
	std::array<int, 4> viewport;
};

}

#endif