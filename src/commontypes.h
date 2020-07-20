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

template <typename FacePointerType, typename PositionType>
class Intersection {

public:
	Intersection(const FacePointerType &_face, const PositionType &_position,
				bool _intersect = true)
	  : face(_face), position(_position), intersect(_intersect) {}

	const FacePointerType &Face() const { return face; }

	const PositionType &Position() const { return position; }

	bool Intersect() const { return intersect; }

private:
	FacePointerType face;
	PositionType position;
	bool intersect;
};

}

#endif
