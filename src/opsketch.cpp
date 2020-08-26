#include "opsketch.h"

#include "optrimesh.h"
#include "opmeshrenderer.h"

#include "utils/debug.h"

skbar::OPSketch::OPSketch(EditableMesh *_mesh) : mesh(_mesh),
                                                 started(false) {

}

bool skbar::OPSketch::Start() {
    started = true;

    return true;
}

bool skbar::OPSketch::Close() {
    started = false;

    return true;
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

            const auto firstQuadFace = trimesh.GetQuadFaceId(data.front().Pointer());
            const auto currentQuadFace = trimesh.GetQuadFaceId(intersection.Pointer());

            const auto sameQuadFace = (firstQuadFace == currentQuadFace);

            if ((data.size() < 7) || (data.size() >= 7 && !sameQuadFace)) {
                const auto[hasPath, result] = trimesh.GetIntersectionsBetween(
                        Intersection<int, Vec3f>(data.back().Pointer(), data.back().Position()),
                        intersection, direction, projection);

                if (hasPath) {
                    for (auto i : result) {
                        data.emplace_back(i.Pointer(), i.Position(), SketchVertex::EType::EDGE);
                    }

                    if (!result.empty()) {
                        data.emplace_back(intersection.Pointer(), intersection.Position(), SketchVertex::EType::FACE);
                    }
                }

                added = hasPath;
            } else {

                const Intersection<int, Vec3f> firstIntersection(data.front().Pointer(), data.front().Position());

                const auto[hasPath, result] = trimesh.GetIntersectionsBetween(intersection, firstIntersection,
                                                                        direction, projection);

                for (auto i : result) {
                    data.emplace_back(i.Pointer(), i.Position(), SketchVertex::EType::EDGE);
                }

                Close();
            }


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
