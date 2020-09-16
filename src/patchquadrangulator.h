//
// Created by cassiano on 03/05/2020.
//

#ifndef SKBAR_PATCHQUADRANGULATOR_H
#define SKBAR_PATCHQUADRANGULATOR_H

#include "opquadmesh.h"

#include "patchgen/generate_topology.hh"

#include <Eigen/Sparse>

#include <vector>

namespace skbar {

class PatchQuadrangulator {

private:
    const static double LAPLACE_CONSTRAINT_WEIGHT;

public:

    static OPQuadMesh
    Quadrangulate(const Eigen::VectorXi &patchSides, const std::vector<OpenMesh::Vec3d> &positions,
                  bool positionsIsClockwise);

    static patchgen::PatchParam ComputeTopology(const Eigen::VectorXi &patchSides, OPQuadMesh &mesh);

    static Eigen::SparseMatrix<double> GetLaplacianMatrix(const OPQuadMesh &mesh, const patchgen::PatchParam &param);

    static Eigen::Matrix<double, -1, 3> GetRightSide(const OPQuadMesh &mesh, const patchgen::PatchParam &param);

    static void SetLaplacianPositions(OPQuadMesh &patch, const std::vector<OpenMesh::Vec3d> &positions,
                                      bool positionsIsClockwise);
};
}

#endif //SKBAR_PATCHQUADRANGULATOR_H
