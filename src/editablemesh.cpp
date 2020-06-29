#include "editablemesh.h"

#include "utils/debug.h"

#include <OpenMesh/Core/IO/MeshIO.hh>

#include <set>

skbar::EditableMesh::EditableMesh() {
}

skbar::EditableMesh::~EditableMesh() {
}

bool skbar::EditableMesh::Load(const std::string &filename) {

    if (!OpenMesh::IO::read_mesh(quadMesh, filename.c_str())) {
        Error("Cannot read file %s", filename.c_str());

        return false;
    }

    // Set data of current quad face id
    for (auto fi : quadMesh.faces()) {
        quadMesh.data(fi).quadFaceData.id = fi.idx();
    }

    QuadMesh tempTriMesh = quadMesh;
    tempTriMesh.triangulate();

    // Assign triangulated mesh to triMesh
    // Direct copy is not allowed because of non-stardard properties
    triMesh.assign(tempTriMesh);

    // Copy data from quad mesh
    for (auto fi : tempTriMesh.faces()) {
        triMesh.data(fi).quadFaceData.id = tempTriMesh.data(fi).quadFaceData.id;
    }
}

std::tuple<OpenMesh::Vec3f, OpenMesh::Vec3f> skbar::EditableMesh::GetBoundingBox() const {
#warning "Implement this"
    // TODO Implement bounding box
}
