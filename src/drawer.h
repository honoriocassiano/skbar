#ifndef DRAWER_H
#define DRAWER_H

#include "commontypes.h"
#include "editablemesh.h"

namespace skbar {

class Drawer {

public:
    Drawer() {}
    virtual ~Drawer() {}

    virtual bool Draw(const Vec3f &origin, const Vec3f &direction) = 0;

    virtual EditableMesh &GetMesh() = 0;

    virtual const EditableMesh &GetMesh() const = 0;

    // virtual Line<Vec3f> GetLine() const = 0;

};
}

#endif /* DRAWER_H */
