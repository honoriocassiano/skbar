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

}

#endif
