#ifndef PROJECTION_H
#define PROJECTION_H

#include "commontypes.h"

namespace skbar {

template <typename T>
class Projection {

public:
	Projection() {}

	virtual ~Projection() {}
	
	virtual std::array<T, 2> Project(const std::array<T, 3> &point) const = 0;

	virtual std::array<T, 3> UnProject(const std::array<T, 2> &point) const = 0;
};

}

#endif