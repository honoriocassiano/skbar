//
// Created by cassiano on 07/10/2020.
//

#ifndef SKBAR_REQUADRANGULATORNEW_H
#define SKBAR_REQUADRANGULATORNEW_H

#include "editablemesh.h"

namespace skbar {

class RequadrangulatorNew {

    struct InOutSKVIndex {
        int in;
        int out;
    };

public:
    static void RequadrangulateAll(EditableMesh &editableMesh);

    static std::map<int, std::vector<InOutSKVIndex>> FindAffectedPatches(const EditableMesh &editableMesh);
};
}

#endif //SKBAR_REQUADRANGULATORNEW_H
