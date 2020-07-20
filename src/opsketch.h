#ifndef OPSKETCH_H
#define OPSKETCH_H

#include "editablemesh.h"

namespace skbar {

class OPSketch : public Sketch{

public:
    OPSketch(EditableMesh *mesh);

    virtual ~OPSketch();

    bool Start() override;

    bool Close() override;

    bool AddPoint(const Line<Vec3f> &ray, const Intersection<int, Vec3f> &intersection) override;

    bool IsStarted() const override;

private:
    EditableMesh *mesh;

    bool started;

};
}

#endif /* OPSKETCH_H */
