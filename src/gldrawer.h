#ifndef GLDRAWER_H
#define GLDRAWER_H

#include "drawer.h"
#include "viewer.h"
#include "baseeventprocessor.h"

namespace skbar {

class GLDrawer : public Drawer {

public:
    GLDrawer(Viewer *viewer);
    virtual ~GLDrawer();

    bool ProcessEvent(const Event &event) override;

    void Reset() override;

    skbar::Intersection<int, skbar::Vec3f> GetClosestIntersection(
        const Line<Vec3f> &ray) override;

    EditableMesh &GetMesh() override { return viewer->GetMesh(); }

    const EditableMesh &GetMesh() const override { return viewer->GetMesh(); }

    const Camera &GetCamera() const { return viewer->GetCamera(); }

    // Line<Vec3f> GetLine() const override;

private:
    Viewer *viewer;

    BaseEventProcessor *processor;

    bool drawing;
};
}

#endif /* GLDRAWER_H */
