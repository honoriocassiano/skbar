#include <OpenMesh/Core/IO/MeshIO.hh>

#include "meshtypes.h"
#include "patchtracer.h"
#include "patchquadrangulator.h"

#include "lp_solve_5.5/lp_lib.h"

using namespace skbar;

int main(int argc, char *argv[]) {

    // The Begginng of a New Era
    QuadMesh mesh;

    if (!OpenMesh::IO::read_mesh(mesh, "/home/cassiano/workspace-cpp/skbar/models/plane.obj")) {
        std::cerr << "read error\n";
        exit(1);
    }

//    std::vector<std::vector<OpenMesh::Vec3d>> sides;
//    lprec* ptr;
//
//    set_verbose(ptr, SEVERE);

    Eigen::VectorXi sides;

    PatchQuadrangulator::ComputeTopology(sides);
//    auto param = patchgen::get_default_parameter(sides);

//    PatchTracer::Trace(mesh);

//    PatchTracer::GetGrid(mesh, 0);



    return 0;
}
