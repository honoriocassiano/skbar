#ifndef DRAWER_H
#define DRAWER_H

#include "commontypes.h"
#include "editablemesh.h"
#include "baseeventprocessor.h"

namespace skbar {

class Drawer {

public:
    Drawer() {}
    virtual ~Drawer() {}

    virtual bool ProcessEvent(const Event &event) = 0;

    virtual bool IsStarted() const = 0;

    virtual void Reset() = 0;

    virtual skbar::Intersection<int, skbar::Vec3f> GetClosestIntersection(
        const Line<Vec3f> &ray) = 0;

    virtual EditableMesh &GetMesh() = 0;

    virtual const EditableMesh &GetMesh() const = 0;

    // virtual Line<Vec3f> GetLine() const = 0;

};
}

#endif /* DRAWER_H */
