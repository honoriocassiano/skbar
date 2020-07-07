#ifndef DRAWER_H
#define DRAWER_H

#include "commontypes.h"

namespace skbar {

class Drawer {

public:
    Drawer() {}
    virtual ~Drawer() {}

    virtual bool Draw(const Vec3f &origin, const Vec3f &direction) = 0;

};
}

#endif /* DRAWER_H */
