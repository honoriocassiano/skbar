#ifndef SKETCH_H
#define SKETCH_H

#include "commontypes.h"

#include <vector>

namespace skbar {

class SketchVertex {

public:

    enum class EType {
        FACE,
        EDGE
    };

    // TODO REMOVE TYPE 'INT' AND ADD AS TEMPLATE
    SketchVertex(const int &_pointer, const Vec3f &_position, const EType &_type)
        : pointer(_pointer), position(_position), type(_type) {}

    virtual ~SketchVertex() {}

    int Pointer() const { return pointer; }

    const Vec3f &Position() const { return position; }

    const EType &Type() const { return type; }

private:
    int pointer;
    Vec3f position;
    EType type;
};

class Sketch {

public:
    Sketch() {}

    virtual ~Sketch() {}

    virtual bool Start() = 0;

    virtual bool Close() = 0;

    /*
		Try to add a new point to the sketch if these conditions is filled:

		- The sketch is started
		- The sketch is not closed
		- The ray intersect the mesh
    */
    virtual bool AddPoint(const Line<Vec3f> &ray, const Intersection<int, Vec3f> &intersection) = 0;

    virtual std::size_t Size() const = 0;

    // TODO Change this return type
    virtual const std::vector<SketchVertex> &Data() const = 0;

    virtual void Render() const = 0;

    virtual bool IsStarted() const = 0;

};
}

#endif /* SKETCH_H */
