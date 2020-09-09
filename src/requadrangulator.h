//
// Created by cassiano on 09/09/2020.
//

#ifndef SKBAR_REQUADRANGULATOR_H
#define SKBAR_REQUADRANGULATOR_H

#include "editablemesh.h"

namespace skbar {

class Requadrangulator {

public:
    explicit Requadrangulator(EditableMesh *mesh);

    virtual ~Requadrangulator() = default;

    void RequadrangulateAll();

    void RequadrangulatePatch(int patch);

private:


private:
    EditableMesh *mesh;
};
}

#endif //SKBAR_REQUADRANGULATOR_H
