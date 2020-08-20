//
// Created by cassiano on 20/08/2020.
//

#ifndef SKBAR_MESHRENDERER_H
#define SKBAR_MESHRENDERER_H

#include "editablemesh.h"
#include "renderoptions.h"

namespace skbar::renderer {

class MeshRenderer {
public:

    MeshRenderer() = default;
    virtual ~MeshRenderer() = default;

    virtual void Render(const EditableMesh &mesh, const Options &options) = 0;
};
}

#endif //SKBAR_MESHRENDERER_H
