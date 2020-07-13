#ifndef SKETCH_H
#define SKETCH_H

#include "commontypes.h"

namespace skbar {

class Sketch {

public:
    Sketch() {}

    virtual ~Sketch() {}

    virtual bool AddPoint(const Line<Vec3f> &ray) = 0;

};
}

#endif /* SKETCH_H */
