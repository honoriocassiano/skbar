//
// Created by cassiano on 29/09/2020.
//

#ifndef SKBAR_TRIANGLEUTILS_H
#define SKBAR_TRIANGLEUTILS_H

#include "commontypes.h"

#include <optional>

namespace skbar::utils {

template <typename T>
std::optional<Vec3f> GetBarycentricCoordinate(const T &position, const T &p0, const T &p1,
                                               const T &p2) {

    // https://gamedev.stackexchange.com/questions/23743/whats-the-most-efficient-way-to-find-barycentric-coordinates
    const auto v0 = Sub(p1, p0);
    const auto v1 = Sub(p2, p0);
    const auto v2 = Sub(position, p0);

    const auto d00 = Dot(v0, v0);
    const auto d01 = Dot(v0, v1);
    const auto d11 = Dot(v1, v1);
    const auto d20 = Dot(v2, v0);
    const auto d21 = Dot(v2, v1);

    const auto denominator = d00 * d11 - d01 * d01;

    Vec3f result;

    result[1] = (d11 * d20 - d01 * d21) / denominator;
    result[2] = (d00 * d21 - d01 * d20) / denominator;
    result[0] = 1.0f - result[1] - result[2];

    return result;
}

}

#endif //SKBAR_TRIANGLEUTILS_H
