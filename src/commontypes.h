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

    // TODO Remove this function
    const T &GetOrigin() const { return origin; }

    // TODO Remove this function
    const T &GetDirection() const { return direction; }

    const T &Origin() const { return origin; }

    const T &Direction() const { return direction; }

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

template <typename PointType>
class Segment {

public:
	Segment(const PointType &_p1, const PointType &_p2) : p1(_p1), p2(_p2) {}
	~Segment() {}

	const PointType &P1() const { return p1; }
	const PointType &P2() const { return p2; }

private:
	PointType p1;
	PointType p2;

};

template <typename PointType>
class Plane {

public:
	Plane(const PointType &_point, const PointType &_normal) : point(_point), normal(_normal) {}
	~Plane() {}

	const PointType &Point() const { return point; }
	const PointType &Normal() const { return normal; }

private:
	PointType point;
	PointType normal;

};

}

#endif
