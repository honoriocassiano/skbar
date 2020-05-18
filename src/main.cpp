#include <OpenMesh/Core/IO/MeshIO.hh>
#include <utils/debug.h>
#include <Eigen/SparseCholesky>

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
    std::vector<OpenMesh::Vec3d> positions;

    std::vector<int> sidesVec{2, 2, 1, 1};
//    std::vector<int> sidesVec{1, 1, 1, 1};

    positions.emplace_back(0, 0, 0);
    positions.emplace_back(0.5, 0, 0);
    positions.emplace_back(1, 0, 0);
    positions.emplace_back(1, 0.5, 0);
    positions.emplace_back(1, 1, 0);
//    positions.emplace_back(0.5, 1, 0);
    positions.emplace_back(0, 1, 0);

    // A simple quad
    sides << 2, 2, 1, 1;
//    sides << 1, 1, 1, 1;


    auto param = PatchQuadrangulator::ComputeTopology(sides, patch);

//    std::cout << std::distance(patch.vertices().begin(), patch.vertices().end());

//    std::cout << param.l << "\n" << std::endl;



//    std::cout << param.get_l_permuted() << "\n" << std::endl;"

    auto L = PatchQuadrangulator::getLaplacianMatrix(patch, param);

    auto b = PatchQuadrangulator::getRightSide(patch, param, positions);

//    Log("%ld", patch.n_vertices());
//    Log("%ld, %ld", L.rows(), L.cols());
//    Log("%ld, %ld", b.rows(), b.cols());

//    Log("%d", param.permutation.id);

//    std::cout << L << "\n" << std::endl;
//    std::cout << b << "\n" << std::endl;

    auto solver = new Eigen::SimplicialCholesky<Eigen::SparseMatrix<double>>(L.transpose() * L);

    Eigen::Matrix<double, -1, 3> x = solver->solve(L.transpose() * b);
    std::cout << x << std::endl;

//    std::cout << laplacianMatrix << std::endl;

//    Eigen::SimplicialCholesky<Eigen::MatrixXd> solver(L.reverse() * L);

//    auto result = solver.solve(L.transpose() * b);


//    auto nSides = sides.size();

//    std::cout << param.permutation.id << std::endl;

//    PatchTracer::Trace(mesh);

//    PatchTracer::GetGrid(mesh, 0);



    return 0;
}
