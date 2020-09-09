#ifndef SKETCH_H
#define SKETCH_H

#include "commontypes.h"

#include <vector>
#include <map>

namespace skbar {

class SketchVertex {

public:

    enum class EType {
        FACE,
        EDGE
    };

    // TODO REMOVE TYPE 'INT' AND ADD AS TEMPLATE
    SketchVertex(const int &_pointer, const Vec3f &_position, const EType &_type,
                 const std::map<int, Vec2f> &_parametricPositionsByPatch)
            : pointer(_pointer), position(_position), type(_type),
              parametricPositionsByPatch(_parametricPositionsByPatch) {}

    virtual ~SketchVertex() {}

    int Pointer() const { return pointer; }

    const Vec3f &Position() const { return position; }

    const EType &Type() const { return type; }

    const std::map<int, Vec2f> &ParametricPositionsByPatch() const { return parametricPositionsByPatch; }

private:
    int pointer;
    Vec3f position;
    EType type;
    std::map<int, Vec2f> parametricPositionsByPatch;
};

class Sketch {

public:
    Sketch() {}

    virtual ~Sketch() {}

    virtual bool Start() = 0;

    virtual bool Close() = 0;

    virtual void Reset() = 0;

    /*
		Try to add a new point to the sketch if these conditions is filled:

		- The sketch is started
		- The sketch is not closed
		- The ray intersect the mesh
    */
    virtual bool AddPoint(const Line <Vec3f> &ray, const skbar::Intersection<int, skbar::Vec3f> &intersection,
                          const Projection<float> &projection) = 0;

    virtual std::size_t Size() const = 0;

    // TODO Change this return type
    virtual const std::vector<SketchVertex> &Data() const = 0;

    virtual bool IsStarted() const = 0;

};
}

#endif /* SKETCH_H */
