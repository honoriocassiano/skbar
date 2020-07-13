#include "intersectionutils.h"

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
        baricentric = Sum(origin, Mul(direction, t));
        return true;
    }

    // Line intersection but not ray intersection
    return false;
}
