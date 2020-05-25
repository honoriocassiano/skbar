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

        if (it.is_boundary()) {
            triplets.emplace_back(size + i, i, LAPLACE_CONSTRAINT_WEIGHT);
            nControlVertices++;
        }
    }

    laplacianMatrix.resize(size + nControlVertices, size);
    laplacianMatrix.setFromTriplets(triplets.begin(), triplets.end());

    return laplacianMatrix;
}

Eigen::Matrix<double, -1, 3> skbar::PatchQuadrangulator::GetRightSide(const QuadMesh &mesh,
                                                                      const patchgen::PatchParam &param,
                                                                      const std::vector<OpenMesh::Vec3d> &positions) {

    auto nv = mesh.n_vertices();

    Eigen::Matrix<double, -1, 3> b(nv + positions.size(), 3);

    b.setZero();

    for (int i = 0; i < positions.size(); i++) {

        auto point = positions[i];

        b(nv + i, 0) = point[0] * LAPLACE_CONSTRAINT_WEIGHT;
        b(nv + i, 1) = point[1] * LAPLACE_CONSTRAINT_WEIGHT;
        b(nv + i, 2) = point[2] * LAPLACE_CONSTRAINT_WEIGHT;
    }

    return b;
}

std::vector<OpenMesh::Vec3d>
skbar::PatchQuadrangulator::GetShiftedPositions(const std::vector<OpenMesh::Vec3d> &positions,
                                                const patchgen::PatchParam &param) {

    std::vector<OpenMesh::Vec3d> shiftedPositions(positions.size());

    // Aux vector to nonCornerPositions
    std::vector<std::size_t> nonCornerPositions(positions.size() - param.get_num_sides());

    // Positions
    std::vector<std::size_t> mapPositions(param.get_num_sides());

    std::size_t nSides = param.get_num_sides();

    for (std::size_t iCorner = 0, iLastCornerSize = 0, posLastNonCorner = 0; iCorner < nSides; iCorner++) {
        mapPositions[iCorner] = iLastCornerSize;

        for (std::size_t iNonCorner = 1; iNonCorner < param.l(iCorner); iNonCorner++) {
            nonCornerPositions[posLastNonCorner] = iLastCornerSize + iNonCorner;

            posLastNonCorner++;
        }

        iLastCornerSize += param.l(iCorner);
    }

    // Add non corners do map
    mapPositions.insert(mapPositions.begin() + param.get_num_sides(), nonCornerPositions.begin(), nonCornerPositions.end());

    for (std::size_t i = 0; i < mapPositions.size(); i++) {
        shiftedPositions[i] = positions[mapPositions[i]];
    }

    return shiftedPositions;
}
