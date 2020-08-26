#ifndef OPSKETCH_H
#define OPSKETCH_H

#include "editablemesh.h"

namespace skbar {

class OPSketch : public Sketch {

public:
    OPSketch(EditableMesh *mesh);

    virtual ~OPSketch() = default;

    bool Start() override;

    bool Close() override;

    void Reset() override;

    bool AddPoint(const Line <Vec3f> &ray, const skbar::Intersection<int, skbar::Vec3f> &intersection,
                  const Projection<float> &projection) override;

    const std::vector<SketchVertex> &Data() const override { return data; }

    std::size_t Size() const override { return data.size(); }

    bool IsStarted() const override;

private:
    EditableMesh *mesh;

    bool started;

    std::vector<SketchVertex> data;

};
}

#endif /* OPSKETCH_H */
