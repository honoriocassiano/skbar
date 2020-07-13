#ifndef COMMON_TYPES_H_
#define COMMON_TYPES_H_

#include <array>

namespace skbar {

// Bounding box to a 3D mesh
typedef std::array<std::array<float, 2>, 3> BBox;

typedef std::array<int, 2> Vec2i;
typedef std::array<float, 2> Vec2f;
typedef std::array<int, 3> Vec3i;
typedef std::array<float, 3> Vec3f;

template <typename T>
class Line {

public:
    Line(const T &_origin, const T &_direction) : origin(_origin), direction(_direction) {}

    virtual ~Line() {}

    const T &GetOrigin() const { return origin; }

    const T &GetDirection() const { return direction; }

private:
    T origin;
    T direction;

};

}

#endif
