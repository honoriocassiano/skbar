//
// Created by cassiano on 03/05/2020.
//

#ifndef SKBAR_PATCHQUADRANGULATOR_H
#define SKBAR_PATCHQUADRANGULATOR_H

#include "meshtypes.h"

#include "patchgen/generate_topology.hh"

#include <vector>
#include <Eigen/Sparse>

namespace skbar {

    class PatchQuadrangulator {

    private:
        const static double LAPLACE_CONSTRAINT_WEIGHT;

    public:

        static patchgen::PatchParam ComputeTopology(const Eigen::VectorXi &patchSides, QuadMesh &mesh);

        static Eigen::SparseMatrix<double> GetLaplacianMatrix(const QuadMesh &mesh, const patchgen::PatchParam &param);

        static Eigen::Matrix<double, -1, 3> GetRightSide(const QuadMesh &mesh, const patchgen::PatchParam &param,
                                                         const std::vector<OpenMesh::Vec3d> &positions);
    };
}

#endif //SKBAR_PATCHQUADRANGULATOR_H
