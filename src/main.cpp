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

using namespace skbar;

int main(int argc, char *argv[]) {

    SDLApplication app("SkBaR", 800, 600);

    app.Initialize();

// //    QuadMesh mesh;
// //
// //    if (!OpenMesh::IO::read_mesh(mesh, "/home/cassiano/workspace-cpp/skbar/models/plane.obj")) {
// //        std::cerr << "read error\n";
// //        exit(1);
// //    }
// 
//     QuadMesh patch;
// 
//     Eigen::VectorXi sides;
//     std::vector<OpenMesh::Vec3d> positions;
// 
//     generate_example_4_4(positions, sides);
// //    generate_example_4_1_complex(positions, sides);
// //    generate_example_4_2(positions, sides);
// //    generate_example_4_1(positions, sides);
// //    generate_example_5_1(positions, sides);
// 
//     auto param = PatchQuadrangulator::ComputeTopology(sides, patch);
// 
//     PatchQuadrangulator::SetLaplacianPositions(patch, positions, false);
// 
//     auto L = PatchQuadrangulator::GetLaplacianMatrix(patch, param);
//     auto b = PatchQuadrangulator::GetRightSide(patch, param);
// 
//     auto solver = new Eigen::SimplicialCholesky<Eigen::SparseMatrix<double>>(L.transpose() * L);
// 
//     Eigen::Matrix<double, -1, 3> x = solver->solve(L.transpose() * b);
// 
//     for (auto it = patch.vertices().begin(); it != patch.vertices().end(); it++) {
//         patch.set_point(*it, OpenMesh::Vec3d(x(it->idx(), 0), x(it->idx(), 1), x(it->idx(), 2)));
//     }
// 
//     OpenMesh::IO::write_mesh(patch, "/home/cassiano/retopo.obj");

    return 0;
}
