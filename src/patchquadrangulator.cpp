//
// Created by cassiano on 03/05/2020.
//


#include <utils/debug.h>
#include "patchquadrangulator.h"

#include "patchgen/decl.hh"

const double skbar::PatchQuadrangulator::LAPLACE_CONSTRAINT_WEIGHT = 10 * 1000;

patchgen::PatchParam skbar::PatchQuadrangulator::ComputeTopology(const Eigen::VectorXi &patchSides, QuadMesh &mesh) {
    patchgen::PatchParam param;

    patchgen::generate_topology<QuadMesh>(patchSides, param, mesh);

    return param;
}

Eigen::SparseMatrix<double> skbar::PatchQuadrangulator::GetLaplacianMatrix(const skbar::QuadMesh &mesh,
                                                                           const patchgen::PatchParam &param) {

    typedef Eigen::Triplet<double> Triplet;
    typedef Eigen::SparseMatrix<double> Matrix;

    Matrix laplacianMatrix;

    auto size = mesh.n_vertices();

//    Eigen::MatrixXd laplacianMatrix(size, size);

    laplacianMatrix.setZero();

    int nControlVertices = 0;
    std::vector<Triplet> triplets;

    // Default laplacian matrix
    for (auto it : mesh.vertices()) {
        auto i = it.idx();

        for (const auto &vn: it.vertices()) {
//            laplacianMatrix(i, vn.idx()) = -1;
            triplets.emplace_back(i, vn.idx(), -1.0);
        }

        triplets.emplace_back(i, i, it.valence());

//        OpenMesh::VertexHandle(it);

//        if (mesh.data(it).patchgen.corner_index != -1) {
        if (it.is_boundary()) {
            triplets.emplace_back(size + i, i, LAPLACE_CONSTRAINT_WEIGHT);
            nControlVertices++;
        }
    }

    auto nsides = param.get_num_sides();

    std::vector<OpenMesh::Vec3d> p;

//    auto lastCorner = 0;
//
//    for (auto i = 0; i < nsides; i++) {
//
//        p.push_back(positions[lastCorner + i]);
//
//        for (int k = 1; k < param.l(i); k++) {
////            std::cout << ab << std::endl;
//
//            p.push_back(positions[lastCorner + i + k]);
//        }
//
//        lastCorner += param.l(i);
//    }

    laplacianMatrix.resize(size + nControlVertices, size);
    laplacianMatrix.setFromTriplets(triplets.begin(), triplets.end());

    return laplacianMatrix;
}

Eigen::Matrix<double, -1, 3> skbar::PatchQuadrangulator::GetRightSide(const QuadMesh &mesh,
                                                                      const patchgen::PatchParam &param,
                                                                      const std::vector<OpenMesh::Vec3d> &positions) {

//    std::vector<double> positions;

    auto perm = param.permutation;
    auto sides = param.get_num_sides();

    auto startPos = perm[0];

//    if (!perm.is_flipped()) {
//        startPos = perm[0];
//    } else {
//        startPos = perm[0];
//    }

    std::vector<OpenMesh::Vec3d> p;

    auto lastCorner = 0;

    for (int i = 0; i < sides; i++) {

        p.push_back(positions[lastCorner + i]);
        Log("%d", lastCorner + i);

        for (int k = 1; k < param.l(i); k++) {
//            std::cout << ab << std::endl;

            p.push_back(positions[lastCorner + i + k]);

            Log("%d", lastCorner + i + k);
        }

        lastCorner += param.l(i) - 1;
    }

    auto nv = mesh.n_vertices();

    Eigen::Matrix<double, -1, 3> b(nv + p.size(), 3);

//        b.resize(nv + p.size());
    b.setZero();

    for (int i = 0; i < p.size(); i++) {

        auto p2 = p[i];

        b(nv + i, 0) = p2[0] * LAPLACE_CONSTRAINT_WEIGHT;
        b(nv + i, 1) = p2[1] * LAPLACE_CONSTRAINT_WEIGHT;
        b(nv + i, 2) = p2[2] * LAPLACE_CONSTRAINT_WEIGHT;
    }

    return b;

}
