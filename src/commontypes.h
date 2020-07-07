#ifndef COMMON_TYPES_H_
#define COMMON_TYPES_H_

#include <array>

namespace skbar {

// Bounding box to a 3D mesh
typedef std::array<std::array<float, 2>, 3> BBox;

}

#endif
