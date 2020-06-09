//
// Created by cassiano on 07/06/2020.
//

#ifndef SKBAR_VECTOR_H
#define SKBAR_VECTOR_H

#include <vector>
#include <string>
#include <sstream>

#include "meshtypes.h"

namespace skbar {

    template<typename T>
    std::string PrintVector(const std::vector<T> &vec) {

        std::stringstream ss;

        ss << "[";

        for (std::size_t i = 0; i < vec.size(); i++) {
            if (i == 0) {
                ss << vec.at(i);
            } else {
                ss << ", " << vec.at(i);
            }
        }

        ss << "]";

        return ss.str();
    }

    void PrintPosition(std::stringstream &stream, const OpenMesh::Vec3d &vec);

    std::string PrintVector(const std::vector<OpenMesh::Vec3d> &vec);

}

#endif //SKBAR_VECTOR_H
