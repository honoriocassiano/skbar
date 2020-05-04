#include <OpenMesh/Core/IO/MeshIO.hh>

#include "meshtypes.h"
#include "patchtracer.h"
#include "patchquadrangulator.h"

#include "lp_solve_5.5/lp_lib.h"

using namespace skbar;

int main(int argc, char *argv[]) {

    // The Begginng of a New Era
//    QuadMesh mesh;
//
//    if (!OpenMesh::IO::read_mesh(mesh, "/home/cassiano/workspace-cpp/skbar/models/plane.obj")) {
//        std::cerr << "read error\n";
//        exit(1);
//    }

    QuadMesh patch;

    Eigen::VectorXi sides(4);

    // A simple quad
    sides << 1, 1, 1, 1;

    PatchQuadrangulator::ComputeTopology(sides, patch);

    std::cout << std::distance(patch.vertices().begin(), patch.vertices().end());

//    PatchTracer::Trace(mesh);

//    PatchTracer::GetGrid(mesh, 0);



    return 0;
}
