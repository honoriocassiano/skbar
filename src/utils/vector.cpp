//
// Created by cassiano on 07/06/2020.
//

#include "vector.h"

namespace skbar {

    void PrintPosition(std::stringstream &stream, const OpenMesh::Vec3d &vec) {
        stream << "(" << vec.data()[0] << ", " << vec.data()[1] << ", " << vec.data()[2] << ")";
    }

    std::string PrintVector(const std::vector<OpenMesh::Vec3d> &vec) {

        std::stringstream ss;

        ss << "[";

        for (std::size_t i = 0; i < vec.size(); i++) {
            if (i == 0) {
                PrintPosition(ss, vec.at(i));
            } else {
                ss << ", ";
                PrintPosition(ss, vec.at(i));
            }
        }

        ss << "]";

        return ss.str();
    }

};