#ifndef INTERSECTIONUTILS_H
#define INTERSECTIONUTILS_H

#include "commontypes.h"

#include <limits>

namespace skbar {

namespace utils {

bool CheckIntersectionTriangleRay(const Line<Vec3f> &ray, const Vec3f &p0, const Vec3f &p1,
        const Vec3f &p2, Vec3f &intersection);

}
}
#endif /* INTERSECTIONUTILS_H */
