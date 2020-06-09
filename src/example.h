//
// Created by cassiano on 25/05/2020.
//

#ifndef SKBAR_EXAMPLE_H
#define SKBAR_EXAMPLE_H

#include "meshtypes.h"
#include <Eigen/Core>

namespace skbar {

    void generate_example_4_0(std::vector<OpenMesh::Vec3d> &positions, Eigen::VectorXi &sides) {

        positions.resize(4);

        positions[0] = OpenMesh::Vec3d(0, 0, 0);
        positions[1] = OpenMesh::Vec3d(1, 0, 0);
        positions[2] = OpenMesh::Vec3d(1, 1, 0);
        positions[3] = OpenMesh::Vec3d(0, 1, 0);

        sides.resize(4);

        sides << 1, 1, 1, 1;
    }

    void generate_example_4_1(std::vector<OpenMesh::Vec3d> &positions, Eigen::VectorXi &sides) {

        positions.resize(6);

        positions[0] = OpenMesh::Vec3d(0, 0, 0);
        positions[1] = OpenMesh::Vec3d(0.5, 0, 0);
        positions[2] = OpenMesh::Vec3d(1, 0, 0);
        positions[3] = OpenMesh::Vec3d(1, 0.5, 0);
        positions[4] = OpenMesh::Vec3d(1, 1, 0);
        positions[5] = OpenMesh::Vec3d(0, 1, 0);

        sides.resize(4);

        sides << 2, 2, 1, 1;
    }

    void generate_example_4_1_complex(std::vector<OpenMesh::Vec3d> &positions, Eigen::VectorXi &sides) {

        positions.resize(8);

        positions[0] = OpenMesh::Vec3d(0, 0, 0);
        positions[1] = OpenMesh::Vec3d(0.25, 0, 0);
        positions[2] = OpenMesh::Vec3d(0.5, 0, 0);
        positions[3] = OpenMesh::Vec3d(1, 0, 0);
        positions[4] = OpenMesh::Vec3d(1, 0.5, 0);
        positions[5] = OpenMesh::Vec3d(1, 0.75, 0);
        positions[6] = OpenMesh::Vec3d(1, 1, 0);
        positions[7] = OpenMesh::Vec3d(0, 1, 0);

        sides.resize(4);

        sides << 3, 3, 1, 1;
    }

    void generate_example_4_2(std::vector<OpenMesh::Vec3d> &positions, Eigen::VectorXi &sides) {

        positions.resize(6);

        positions[0] = OpenMesh::Vec3d(0, 0, 0);
        positions[1] = OpenMesh::Vec3d(0.33, 0, 0);
        positions[2] = OpenMesh::Vec3d(0.66, 0, 0);
        positions[3] = OpenMesh::Vec3d(1, 0, 0);
        positions[4] = OpenMesh::Vec3d(1, 1, 0);
        positions[5] = OpenMesh::Vec3d(0, 1, 0);

        sides.resize(4);

        sides << 3, 1, 1, 1;
    }

    void generate_example_4_4(std::vector<OpenMesh::Vec3d> &positions, Eigen::VectorXi &sides) {

        positions.resize(8);

        positions[0] = OpenMesh::Vec3d(0, 0, 0);
        positions[1] = OpenMesh::Vec3d(0.25, 0, 0);
        positions[2] = OpenMesh::Vec3d(0.5, 0, 0);
        positions[3] = OpenMesh::Vec3d(0.75, 0, 0);
        positions[4] = OpenMesh::Vec3d(1, 0, 0);
        positions[5] = OpenMesh::Vec3d(1, 0.5, 0);
        positions[6] = OpenMesh::Vec3d(1, 1, 0);
        positions[7] = OpenMesh::Vec3d(0, 1, 0);

        sides.resize(4);

        sides << 4, 2, 1, 1;
    }

    void generate_example_5_0(std::vector<OpenMesh::Vec3d> &positions, Eigen::VectorXi &sides) {

        positions.resize(6);

        positions[0] = OpenMesh::Vec3d(0.25, 0, 0);
        positions[1] = OpenMesh::Vec3d(0.5, 0, 0);
        positions[2] = OpenMesh::Vec3d(0.75, 0, 0);
//        positions[1] = OpenMesh::Vec3d(0.5, 0, 0);
        positions[3] = OpenMesh::Vec3d(1, 0.5, 0);
//        positions[3] = OpenMesh::Vec3d(1, 0.5, 0);
        positions[4] = OpenMesh::Vec3d(0.5, 1, 0);
        positions[5] = OpenMesh::Vec3d(0, 0.5, 0);

        sides.resize(5);

        sides << 2, 1, 1, 1, 1;
    }

    void generate_example_5_1(std::vector<OpenMesh::Vec3d> &positions, Eigen::VectorXi &sides) {

        positions.resize(8);

        positions[0] = OpenMesh::Vec3d(0.1, 0, 0);
        positions[1] = OpenMesh::Vec3d(0.3, 0, 0);
        positions[2] = OpenMesh::Vec3d(0.5, 0, 0);
        positions[3] = OpenMesh::Vec3d(0.7, 0, 0);
        positions[4] = OpenMesh::Vec3d(0.9, 0, 0);
        positions[5] = OpenMesh::Vec3d(1, 0.5, 0);
        positions[6] = OpenMesh::Vec3d(0.5, 1, 0);
        positions[7] = OpenMesh::Vec3d(0, 0.5, 0);

        sides.resize(5);

        sides << 4, 1, 1, 1, 1;
    }
}

#endif //SKBAR_EXAMPLE_H
