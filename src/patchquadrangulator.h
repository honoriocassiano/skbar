//
// Created by cassiano on 03/05/2020.
//

#ifndef SKBAR_PATCHQUADRANGULATOR_H
#define SKBAR_PATCHQUADRANGULATOR_H

#include "meshtypes.h"

#include "patchgen/generate_subtopology.hh"

#include <vector>

namespace skbar {

    class PatchQuadrangulator {

    public:

        static void ComputeTopology(const Eigen::VectorXi &patchSides);
    };
}

#endif //SKBAR_PATCHQUADRANGULATOR_H
