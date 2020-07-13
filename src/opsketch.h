#ifndef OPSKETCH_H
#define OPSKETCH_H

#include "editablemesh.h"

namespace skbar {

class OPSketch : public Sketch{

public:
    OPSketch(EditableMesh *mesh);

    virtual ~OPSketch();

    bool AddPoint(const Line<Vec3f> &ray) override;

private:
    EditableMesh *mesh;

};
}

#endif /* OPSKETCH_H */
