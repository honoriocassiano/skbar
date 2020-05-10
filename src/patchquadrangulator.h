//
// Created by cassiano on 03/05/2020.
//

#ifndef SKBAR_PATCHQUADRANGULATOR_H
#define SKBAR_PATCHQUADRANGULATOR_H

#include "meshtypes.h"

#include "patchgen/generate_topology.hh"

#include <vector>

namespace skbar {

    class PatchQuadrangulator {

    public:

        static patchgen::PatchParam ComputeTopology(const Eigen::VectorXi &patchSides, QuadMesh &mesh);

        static Eigen::MatrixXi getLaplacianMatrix(const QuadMesh &mesh);
    };
}

#endif //SKBAR_PATCHQUADRANGULATOR_H
