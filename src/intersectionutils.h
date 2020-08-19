#ifndef INTERSECTIONUTILS_H
#define INTERSECTIONUTILS_H

#include "commontypes.h"

#include <limits>

namespace skbar {

namespace utils {

bool CheckIntersectionTriangleRay(const Line<Vec3f> &ray, const Vec3f &p0, const Vec3f &p1,
        const Vec3f &p2, Vec3f &intersection);

bool CheckIntersectionPlaneLine(const Plane<Vec3f> &plane, const Line<Vec3f> &line, Vec3f &intersection);

bool CheckIntersectionSegmentSegment(const Segment<Vec2f> &s1, const Segment<Vec2f> &s2, Vec2f &intersection);

}
}
#endif /* INTERSECTIONUTILS_H */
