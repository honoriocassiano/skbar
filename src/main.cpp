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
#include "patchquadrangulator.h"
#include "utils/openmesh.h"

using namespace skbar;

std::vector<Vec3f> GetSketch() {

    std::vector<Vec3f> result;

    result.push_back(Vec3f{0, 0.5, 0.0});
    result.push_back(Vec3f{0.2, 0.6, 0.0});
    result.push_back(Vec3f{0.5, 0.43, 0.0});
    result.push_back(Vec3f{0.68, 0.51, 0.0});
    result.push_back(Vec3f{0.72, 0.37, 0.0});
    result.push_back(Vec3f{0.65, 0.2, 0.0});
    result.push_back(Vec3f{0.7, 0, 0.0});

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

    return 0;
}
