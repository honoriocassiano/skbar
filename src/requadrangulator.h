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
    std::vector<std::vector<Vec2f>> FindSidesOfPatch(const SketchVertex &firstSV, const SketchVertex &lastSV) const;

    static std::vector<int> FindCommonPatches(const SketchVertex &sv0, const SketchVertex &sv1) ;


private:
    EditableMesh *editableMesh;
};
}

#endif //SKBAR_REQUADRANGULATOR_H
