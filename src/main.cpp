// #include <OpenMesh/Core/IO/MeshIO.hh>
// #include <utils/debug.h>
// #include <Eigen/SparseCholesky>
//
// #include "meshtypes.h"
// #include "patchtracer.h"
// #include "patchquadrangulator.h"
//
// #include "example.h"

#include "sdlapplication.h"

#include "editablemesh.h"
#include "utils/debug.h"
#include "vector.h"
#include "quadmesh.h"
#include "trimesh.h"

#include "intersectionutils.h"
#include "opquadmesh.h"

using namespace skbar;

// Sketch data
enum class IntersectionType {
    NONE,
    EDGE,
    FACE
};

struct SketchPointMeta {
    IntersectionType type;
    int index;
    bool onPatchBorder;

    SketchPointMeta(IntersectionType _type, int _index, bool _onPatchBorder)
            : type(_type), index(_index), onPatchBorder(_onPatchBorder) {}

    SketchPointMeta()
            : type(IntersectionType::NONE), index(-1), onPatchBorder(false) {}
};

struct SketchPoint {
    Vec3f position;

    SketchPointMeta metadata;

    SketchPoint(const Vec3f &_position, bool _isPatchEdge, bool _isEdge, int index)
            : position(_position) {

        this->metadata.onPatchBorder = _isPatchEdge;
        this->metadata.type = _isEdge ? IntersectionType::EDGE : IntersectionType::FACE;
        this->metadata.index = index;
    }
};

// Polygon data
enum class PolygonVertexType {
    NONE,
    SKETCH,
    MESH,
};

struct PolygonVertex {
    Vec3f position;
    int index;
    PolygonVertexType type;

    PolygonVertex(const Vec3f &_position, const int _index, const PolygonVertexType _type)
            : position(_position), index(_index), type(_type) {}

    PolygonVertex()
            : position(), index(-1), type(PolygonVertexType::NONE) {}
};

struct Polygon {
    std::vector<std::vector<PolygonVertex>> sides;
    int patch;
    bool clockwise;

    Polygon(std::vector<std::vector<PolygonVertex>> _sides, int _patch, bool _clockwise)
            : sides(std::move(_sides)), patch(_patch), clockwise(_clockwise) {}

    Polygon(const std::vector<std::vector<PolygonVertex>> &&_sides, int _patch, bool _clockwise)
            : sides(_sides), patch(_patch), clockwise(_clockwise) {}
};


// TODO Implement this function
int GetCommonPatch(const OPQuadMesh::QuadMeshImpl &mesh, int v1Id, int v2Id) {
    return 0;
}

std::map<int, std::vector<SketchPoint>>
SegmentSketchByPatch(const OPQuadMesh::QuadMeshImpl &mesh, const std::vector<SketchPoint> &sketch) {
    auto firstIndex = -1;

    for (unsigned i = 0; i < sketch.size(); i++) {
        if (sketch.at(i).metadata.onPatchBorder) {
            firstIndex = static_cast<int>(i);
        }
    }

    std::map<int, std::vector<SketchPoint>> result;

    // If it's a polygon with hole
    if (firstIndex == -1) {
        // TODO Find patch here
        result.insert_or_assign(0, result);

        return result;
    }

    auto currentIndex = firstIndex;

    do {

        std::vector<SketchPoint> segment;

        bool finishedPatch = false;

        while (!finishedPatch) {

            if ((sketch.at(currentIndex).metadata.onPatchBorder) && segment.size() > 1) {
                finishedPatch = true;
            } else {
                // Add first vertex on patch
                segment.push_back(sketch.at(currentIndex));

                currentIndex = (currentIndex + 1) % sketch.size();
            }
        }

        const int patch = GetCommonPatch(mesh, segment.at(0).metadata.index,
                                         segment.at(segment.size() - 1).metadata.index);

        result.insert_or_assign(patch, std::move(segment));

        // TODO Remove this
        break;

    } while (currentIndex != firstIndex);

    return result;
}

std::vector<SketchPoint> GetSketch() {

    std::vector<SketchPoint> result;

    result.emplace_back(Vec3f{0.0, 0.6, 0.0}, true, true, 9);
    result.emplace_back(Vec3f{0.3, 0.6, 0.0}, false, false, 3);
    result.emplace_back(Vec3f{0.36, 0.5, 0.0}, false, true, 2);
    result.emplace_back(Vec3f{0.43, 0.43, 0.0}, false, false, 0);
    result.emplace_back(Vec3f{0.5, 0.32, 0.0}, false, true, 1);
    result.emplace_back(Vec3f{0.57, 0.42, 0.0}, false, false, 1);
    result.emplace_back(Vec3f{0.69, 0.5, 0.0}, false, true, 6);
    result.emplace_back(Vec3f{0.77, 0.81, 0.0}, false, false, 3);
    result.emplace_back(Vec3f{1.0, 0.72, 0.0}, true, true, 10);

    return result;
}

int main(int argc, char *argv[]) {

//    SDLApplication app("SkBaR", 1024, 768);
//
//    app.Initialize();
//
//    app.LoadMesh("/home/cassiano/Documents/workspace-cpp/skbar/models/3holes_quad.obj");
//    // app.LoadMesh("/home/cassiano/Documents/workspace-cpp/skbar/models/plane.obj");
//
//    app.Run();

    EditableMesh mesh;

    mesh.Load("../models/parametric_plane.obj");

    const auto &quadmesh = dynamic_cast<const OPQuadMesh &>(mesh.GetQuad()).Get();

    auto sketch = GetSketch();

    const auto segmented = SegmentSketchByPatch(quadmesh, sketch);

    return 0;
}
