//
// Created by cassiano on 07/10/2020.
//

#ifndef SKBAR_MAP_H
#define SKBAR_MAP_H

#include <set>
#include <map>

namespace skbar::utils {

template<typename T, typename U>
std::set<T> ExtractKeys(const std::map<T, U> &map) {
    std::set<T> result;

    for (const auto &entry : map) {
        result.insert(entry.first);
    }

    return result;
}

}

#endif //SKBAR_MAP_H
