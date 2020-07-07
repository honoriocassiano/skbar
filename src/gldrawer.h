#ifndef GLDRAWER_H
#define GLDRAWER_H

#include "drawer.h"
#include "editablemesh.h"

namespace skbar {

class GLDrawer : public Drawer {

public:
    GLDrawer(EditableMesh *mesh);
    virtual ~GLDrawer();

    bool Draw(const Vec3f &origin, const Vec3f &direction) override;

private:
    EditableMesh *mesh;

};
}

#endif /* GLDRAWER_H */
