#include "editablemesh.h"

#include "opquadmesh.h"
#include "optrimesh.h"
#include "opsketch.h"
#include "utils/debug.h"

#include <set>

skbar::EditableMesh::EditableMesh() {

    quadMesh = new skbar::OPQuadMesh();
    triMesh = new skbar::OPTriMesh();
    sketch = new OPSketch(this);
}

skbar::EditableMesh::~EditableMesh() {
    delete quadMesh;
    delete triMesh;
    delete sketch;

    quadMesh = nullptr;
    triMesh = nullptr;
    sketch = nullptr;
}

bool skbar::EditableMesh::Load(const std::string &filename) {

    quadMesh->Load(filename);
    triMesh->Load(*quadMesh);
}

skbar::BBox skbar::EditableMesh::GetBBox() const {
    return quadMesh->GetBBox();
}

void skbar::EditableMesh::Render() const {
    triMesh->Render();
}
