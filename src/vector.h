#ifndef VECTOR_UTILS_H_
#define VECTOR_UTILS_H_

#include <array>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace skbar {

typedef std::array<int, 2> Vec2i;
typedef std::array<float, 2> Vec2f;
typedef std::array<int, 3> Vec3i;
typedef std::array<float, 3> Vec3f;

template<typename T, std::size_t N>
inline constexpr T Norm(const std::array<T, N> &vec) {
    T sum = std::accumulate(vec.begin(), vec.end(), (T) 0, [](T s, T n) { return n*n + s; });

    return std::sqrt(sum);
}

template<typename T, std::size_t N>
constexpr std::array<T, N> Normalize(const std::array<T, N> &vec) {
    T norm = Norm(vec);
    std::array<T, N> normalized{0};

    for (std::size_t i = 0; i < N; i++) {
        normalized[i] = vec[i] / norm;
    }

    return normalized;
}

template<typename T>
inline constexpr std::array<T, 3> Cross(const std::array<T, 3> &v1, const std::array<T, 3> &v2) {

    return std::array<T, 3> {
        v1[1]*v2[2] - v1[2]*v2[1],
        v1[2]*v2[0] - v1[0]*v2[2],
        v1[0]*v2[1] - v1[1]*v2[0]
    };
}

template<typename T, std::size_t N>
constexpr T Dot(const std::array<T, N> &v1, const std::array<T, N> &v2) {
    T sum = 0;

    for (std::size_t i = 0; i < N; i++) {
        sum += v1[i] * v2[i];
    }

    return sum;
}

template<typename T, std::size_t N>
constexpr std::array<T, N> Mul(const std::array<T, N> &v1, const T &v2) {

    std::array<T, N> result {0};

    for (std::size_t i = 0; i < N; i++) {
        result[i] = v1[i] * v2;
    }

    return result;
}

template<typename T, std::size_t N>
constexpr std::array<T, N> Div(const std::array<T, N> &v1, const T &v2) {

    std::array<T, N> result {0};

    for (std::size_t i = 0; i < N; i++) {
        result[i] = v1[i] / v2;
    }

    return result;
}

template<typename T, std::size_t N>
constexpr std::array<T, N> Sum(const std::array<T, N> &v1, const std::array<T, N> &v2) {

    std::array<T, N> result {0};

    for (std::size_t i = 0; i < N; i++) {
        result[i] = v1[i] + v2[i];
    }

    return result;
}

template<typename T, std::size_t N>
constexpr std::array<T, N> Sub(const std::array<T, N> &v1, const std::array<T, N> &v2) {

    std::array<T, N> result {0};

    for (std::size_t i = 0; i < N; i++) {
        result[i] = v1[i] - v2[i];
    }

    return result;
}

#ifdef VECTOR_SET_OPERATORS

template<typename T>
inline constexpr std::array<T, 3> operator^(const std::array<T, 3> &v1, const std::array<T, 3> &v2) {
    return Cross(v1, v2);
}

template<typename T, std::size_t N>
inline constexpr T operator*(const std::array<T, N> &v1, const std::array<T, N> &v2) {
    return Dot(v1, v2);
}

template<typename T, std::size_t N>
inline constexpr std::array<T, N> operator*(const std::array<T, N> &v1, const T &v2) {
    return Mul(v1, v2);
}

template<typename T, std::size_t N>
inline constexpr std::array<T, N> operator/(const std::array<T, N> &v1, const T &v2) {
    return Div(v1, v2);
}

template<typename T, std::size_t N>
constexpr std::array<T, N> operator+(const std::array<T, N> &v1, const std::array<T, N> &v2) {
    return Sum(v1, v2);
}

template<typename T, std::size_t N>
constexpr std::array<T, N> operator-(const std::array<T, N> &v1, const std::array<T, N> &v2) {
    return Sub(v1, v2);
}

#endif

}

#endif
