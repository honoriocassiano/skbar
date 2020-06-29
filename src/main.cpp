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

using namespace skbar;

int main(int argc, char *argv[]) {

    // SDLApplication app("SkBaR", 800, 600);
    //
    // app.Initialize();
    // app.Run();

    EditableMesh mesh;

    mesh.Load("../models/3holes_quad.obj");

    return 0;
}
