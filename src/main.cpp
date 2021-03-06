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

using namespace skbar;

int main(int argc, char *argv[]) {

    SDLApplication app("SkBaR", 800, 600);

    app.Initialize();

    app.LoadMesh("/home/cassiano/Documents/workspace-cpp/skbar/models/3holes_quad.obj");

    app.Run();

    // EditableMesh mesh;
    //
    // mesh.Load("../models/3holes_quad.obj");
    //
    // auto [min, max] = mesh.GetBoundingBox();
    //
    // std::cout << min << std::endl;
    // std::cout << max << std::endl;

    return 0;
}
