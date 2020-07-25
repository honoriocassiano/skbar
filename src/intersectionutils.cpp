#include "intersectionutils.h"

#include "constants.h"
#include "vector.h"

bool skbar::utils::CheckIntersectionTriangleRay(
        const skbar::Line<skbar::Vec3f> &ray,
        const skbar::Vec3f &p0, const skbar::Vec3f &p1,
        const skbar::Vec3f &p2, skbar::Vec3f &intersection) {

    // Möller–Trumbore intersection algorithm
    // Found at https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm

    float epsilon = std::numeric_limits<float>::epsilon();

    auto origin = ray.GetOrigin();
    auto direction = ray.GetDirection();

    Vec3f edge1, edge2;
    Vec3f h, s, q;

    float a, f, u, v;

    edge1 = Sub(p1, p0);
    edge2 = Sub(p2, p0);

    h = Cross(direction, edge2);
    a = Dot(edge1, h);

    if (a > -epsilon && a < epsilon) {
        // Ray is parallell to the triangle
        return false;
    }

    f = 1.0f / a;
    s = Sub(origin, p0);
    u = f * Dot(s, h);

    if (u < 0 || u > 1) {
        return false;
    }

    q = Cross(s, edge1);
    v = f * Dot(direction, q);

    if (v < 0 || (u + v) > 1) {
        return false;
    }

    float t = f * Dot(edge2, q);

    if (t > epsilon) {
        intersection = Sum(origin, Mul(direction, t));
        return true;
    }

    // Line intersection but not ray intersection
    return false;
}

bool skbar::utils::CheckIntersectionPlaneLine(const skbar::Plane<Vec3f> &plane,
        const skbar::Line<Vec3f> &line, Vec3f &intersection) {

    auto dotNormalDir = Dot(plane.Normal(), line.Direction());

    // Line is parallel to the plane
    // Has zero or infinite intersections
    if ((dotNormalDir > -constants::epsilon) && (dotNormalDir < constants::epsilon)) {
        return false;
    }

    // Find constant term in plane equation to find intersection point
    auto d = -Dot(plane.Normal(), plane.Point());

    auto t = (-d - Dot(plane.Normal(), line.Origin())) / Dot(plane.Normal(), line.Direction());

    intersection = Sum(line.Origin(), Mul(line.Direction(), t));

    return true;
}

bool skbar::utils::CheckIntersectionSegmentSegment(const skbar::Segment<Vec2f> &s1,
        const skbar::Segment<Vec2f> &s2, Vec2f &intersection) {

    float slopeS1;
    float slopeS2;

    // Grant that slopes is always defined
    if (s1.P2()[0] != s1.P1()[0]) {
        slopeS1 = (s1.P2()[1] - s1.P1()[1]) / (s1.P2()[0] - s1.P1()[0]);
    } else {
        slopeS1 = (s1.P2()[1] - s1.P1()[1]) / constants::epsilon;
    }

    if (s2.P2()[0] != s2.P1()[0]) {
        slopeS2 = (s2.P2()[1] - s2.P1()[1]) / (s2.P2()[0] - s2.P1()[0]);
    } else {
        slopeS2 = (s2.P2()[1] - s2.P1()[1]) / constants::epsilon;
    }

    // Segments are parallel
    if (abs(slopeS1 - slopeS2) <= constants::epsilon ) {
        return false;
    }

    float constantS1 = s1.P1()[1] - (slopeS1 * s1.P1()[0]);
    float constantS2 = s2.P1()[1] - (slopeS2 * s2.P1()[0]);

    float x = (constantS2 - constantS1) / (slopeS1 - slopeS2);
    float y = (slopeS1 * x) + constantS1;

    // If x or y is out of bounds of delimiter points, then has a line intersection but not a segment intersection
    bool onSegment =
        (std::min(s1.P1()[0], s1.P2()[0]) <= x) && (x <= std::max(s1.P1()[0], s1.P2()[0])) &&
        (std::min(s1.P1()[1], s1.P2()[1]) <= y) && (y <= std::max(s1.P1()[1], s1.P2()[1]));

    if (onSegment) {
        intersection = Vec2f{x, y};
    }

    return onSegment;
}