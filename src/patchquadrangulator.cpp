//
// Created by cassiano on 03/05/2020.
//

#include "patchquadrangulator.h"

#include "patchgen/decl.hh"

void skbar::PatchQuadrangulator::ComputeTopology(const Eigen::VectorXi &patchSides, QuadMesh &mesh) {
    patchgen::PatchParam param;

    patchgen::generate_topology<QuadMesh>(patchSides, param, mesh);
}
