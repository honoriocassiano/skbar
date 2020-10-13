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

using namespace skbar;

struct SketchPoint {
    Vec3f position;
    bool isPatchEdge;
    bool isEdge;

    SketchPoint(const Vec3f &_position, bool _isPatchEdge, bool _isEdge)
            : position(_position), isPatchEdge(_isPatchEdge), isEdge(_isEdge) {}
};

std::vector<SketchPoint> GetSketch() {

    std::vector<SketchPoint> result;

    result.emplace_back(Vec3f{0.0, 0.6, 0.0}, true, true);
    result.emplace_back(Vec3f{0.3, 0.6, 0.0}, false, false);
    result.emplace_back(Vec3f{0.36, 0.5, 0.0}, false, true);
    result.emplace_back(Vec3f{0.43, 0.43, 0.0}, false, false);
    result.emplace_back(Vec3f{0.5, 0.32, 0.0}, false, true);
    result.emplace_back(Vec3f{0.57, 0.42, 0.0}, false, false);
    result.emplace_back(Vec3f{0.69, 0.5, 0.0}, false, true);
    result.emplace_back(Vec3f{1.0, 0.72, 0.0}, true, true);

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

    auto sketch = GetSketch();


    return 0;
}
