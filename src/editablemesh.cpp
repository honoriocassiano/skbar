#include "editablemesh.h"

#include "opquadmesh.h"
#include "optrimesh.h"
#include "utils/debug.h"

#include <set>

skbar::EditableMesh::EditableMesh() : quadMesh(new skbar::OPQuadMesh()), triMesh(new skbar::OPTriMesh()) {
}

skbar::EditableMesh::~EditableMesh() {
    delete quadMesh;
    delete triMesh;

    quadMesh = nullptr;
    triMesh = nullptr;
}

bool skbar::EditableMesh::Load(const std::string &filename) {

    quadMesh->Load(filename);
    triMesh->Load(*quadMesh);
}

skbar::BBox skbar::EditableMesh::GetBBox() const {
    return quadMesh->GetBBox();
}
