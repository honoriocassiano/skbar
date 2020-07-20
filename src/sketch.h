#ifndef SKETCH_H
#define SKETCH_H

#include "commontypes.h"

namespace skbar {

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

    virtual bool IsStarted() const = 0;

private:
	bool started;

};
}

#endif /* SKETCH_H */
