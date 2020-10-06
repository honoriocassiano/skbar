//
// Created by cassiano on 12/08/2020.
//

#ifndef SKBAR_OPENMESH_H
#define SKBAR_OPENMESH_H

#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <array>

namespace skbar::utils {

template<typename T, int N>
std::array<T, N> ToStdVector(const OpenMesh::VectorT<T, N> &vector) {
    std::array<T, N> result;

    for (auto i = 0; i < N; i++) {
        result[i] = vector[i];
    }

    return result;
}

template<typename T, unsigned long N>
OpenMesh::VectorT<T, N> ToOPVector(const std::array<T, N> &vector) {
    OpenMesh::VectorT<T, N> result;

    for (auto i = 0; i < N; i++) {
        result[i] = vector[i];
    }

    return result;
}

}

#endif //SKBAR_OPENMESH_H
