//
// Created by cassiano on 03/05/2020.
//

#include "patchquadrangulator.h"

#include "patchgen/decl.hh"

patchgen::PatchParam skbar::PatchQuadrangulator::ComputeTopology(const Eigen::VectorXi &patchSides, QuadMesh &mesh) {
    patchgen::PatchParam param;

    patchgen::generate_topology<QuadMesh>(patchSides, param, mesh);

    return param;
}

Eigen::MatrixXi skbar::PatchQuadrangulator::getLaplacianMatrix(const skbar::QuadMesh &mesh) {

    auto size = std::distance(mesh.vertices().begin(), mesh.vertices().end());

    Eigen::MatrixXi laplacianMatrix(size, size);

    laplacianMatrix.setZero();

    for (auto it = mesh.vertices().begin(); it != mesh.vertices().end(); it++) {
        auto i = it->idx();

        for (const auto &vn: it->vertices()) {
            laplacianMatrix(i, vn.idx()) = -1;
        }

        laplacianMatrix(i, i) = mesh.valence(*it);
    }

    return laplacianMatrix;
}
