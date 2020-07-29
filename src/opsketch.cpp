#include "opsketch.h"

#include "optrimesh.h"
#include "opmeshrenderer.h"

#include "utils/debug.h"

skbar::OPSketch::OPSketch(EditableMesh *_mesh) : mesh(_mesh),
    started(false) {

}

skbar::OPSketch::~OPSketch() {

}

bool skbar::OPSketch::Start() {
    started = true;

    // TODO Reset sketch here
}
bool skbar::OPSketch::Close() {
    started = false;
}

void skbar::OPSketch::Reset() {
    data.clear();
}

bool skbar::OPSketch::AddPoint(const skbar::Line<skbar::Vec3f> &ray,
        const skbar::Intersection<int, skbar::Vec3f> &intersection,
        const Line<Vec3f> &direction, const Projection<float> &projection) {

    bool added = false;

    // Log("Face after: %d", intersection.Pointer());

    if (started) {

        auto &trimesh = dynamic_cast<OPTriMesh &>(mesh->GetTri());

        #warning "TODO Uncomment this"
        // added = trimesh.AddPointOnIntersection(intersection);

        if (!data.empty()) {
            auto [hasPath, result] = trimesh.GetIntersectionsBetween(Intersection<int, Vec3f>(data.back().Pointer(), data.back().Position()),
                                                                 intersection, direction, projection);

            Log("%d: %lu", hasPath, result.size());

            if (hasPath) {
               for (auto i : result) {
                   data.emplace_back(i.Pointer(), i.Position(), SketchVertex::EType::EDGE);
               }

                data.emplace_back(intersection.Pointer(), intersection.Position(), SketchVertex::EType::FACE);
            }

            added = hasPath;
        } else {

            #warning "TODO Change this face type"
            data.emplace_back(intersection.Pointer(), intersection.Position(), SketchVertex::EType::FACE);

            added = true;
        }
    }

    return added;
}

bool skbar::OPSketch::IsStarted() const {
    return started;
}

void skbar::OPSketch::Render() const {
    renderer::RenderSketch(*this, !started);
}