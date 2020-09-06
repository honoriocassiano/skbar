#include "opsketch.h"

#include "optrimesh.h"
#include "opmeshrenderer.h"
#include "vector.h"
#include "utils/openmesh.h"

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

bool skbar::OPSketch::AddPoint(const Line<Vec3f> &ray,
                               const skbar::Intersection<int, skbar::Vec3f> &intersection,
                               const Projection<float> &projection) {

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
                        intersection, ray, projection);

                if (hasPath) {
                    for (auto i : result) {

                        const auto parametricPositions = GetParametricPositions(i.Pointer(), i.Position(),
                                                                                SketchVertex::EType::EDGE);

                        data.emplace_back(i.Pointer(), i.Position(), SketchVertex::EType::EDGE, parametricPositions);
                    }

                    if (!result.empty()) {
                        const auto parametricPositions = GetParametricPositions(intersection.Pointer(),
                                                                                intersection.Position(),
                                                                                SketchVertex::EType::FACE);

                        data.emplace_back(intersection.Pointer(), intersection.Position(), SketchVertex::EType::FACE,
                                          parametricPositions);
                    }
                }

                added = hasPath;
            } else {

                const Intersection<int, Vec3f> firstIntersection(data.front().Pointer(), data.front().Position());

                const auto[hasPath, result] = trimesh.GetIntersectionsBetween(intersection, firstIntersection,
                                                                              ray, projection);

                for (auto i : result) {
                    const auto parametricPositions = GetParametricPositions(i.Pointer(), i.Position(),
                                                                            SketchVertex::EType::EDGE);

                    data.emplace_back(i.Pointer(), i.Position(), SketchVertex::EType::EDGE, parametricPositions);
                }

                Close();
            }


        } else {

#warning "TODO Change this face type"
            const auto parametricPositions = GetParametricPositions(intersection.Pointer(), intersection.Position(),
                                                                    SketchVertex::EType::FACE);

            data.emplace_back(intersection.Pointer(), intersection.Position(), SketchVertex::EType::FACE,
                              parametricPositions);

            added = true;
        }
    }

    return added;
}

bool skbar::OPSketch::IsStarted() const {
    return started;
}

std::map<int, skbar::Vec2f>
skbar::OPSketch::GetParametricPositions(int pointer, const skbar::Vec3f &position,
                                        const skbar::SketchVertex::EType &type) const {

    std::map<int, Vec2f> result;

    const auto trimesh = dynamic_cast<const OPTriMesh &>(mesh->GetTri()).Get();
    const auto quadmesh = dynamic_cast<const OPQuadMesh &>(mesh->GetQuad()).Get();

    if (type == SketchVertex::EType::FACE) {
        // TODO Implement this
    } else if (type == SketchVertex::EType::EDGE) {

        const auto edge = OpenMesh::SmartEdgeHandle(pointer, &trimesh);

        const auto v0 = OpenMesh::SmartVertexHandle(edge.v0().idx(), &quadmesh);
        const auto v1 = OpenMesh::SmartVertexHandle(edge.v1().idx(), &quadmesh);

        const auto v0Data = quadmesh.data(v0).quadVertexData.patchParametrizations;
        const auto v1Data = quadmesh.data(v1).quadVertexData.patchParametrizations;

        const auto v0Position = utils::ToStdVector(quadmesh.point(v0));
        const auto v1Position = utils::ToStdVector(quadmesh.point(v1));

        const auto edgeLength = Norm(Sub(v1Position, v0Position));

        const auto A = Norm(Sub(position, utils::ToStdVector(quadmesh.point(v0)))) / edgeLength;
        const auto B = 1 - A;

        std::vector<int> patchesToCheck;

        for (const auto &entry : v0Data) {
            const auto patch = entry.first;

            if (v1Data.find(patch) != v1Data.end()) {
                patchesToCheck.push_back(patch);
            }
        }

        assert((patchesToCheck.size() > 0) && "Must have at least one common patch");

        for (const auto &patch : patchesToCheck) {

            const auto v0ParametricPosition = quadmesh.data(v0).quadVertexData.patchParametrizations.at(patch);
            const auto v1ParametricPosition = quadmesh.data(v1).quadVertexData.patchParametrizations.at(patch);

            const auto parametricPosition = Sum(Mul(v0ParametricPosition, A), Mul(v1ParametricPosition, B));

            result[patch] = parametricPosition;
        }
    }

    return result;
}
