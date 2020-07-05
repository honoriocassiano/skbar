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

skbar::BBox skbar::EditableMesh::GetBBox() const {

    OpenMesh::Vec3f bbMin, bbMax;

    auto it = quadMesh.vertices().begin();

    bbMin = bbMax = quadMesh.point(it);

    for (it++; it != quadMesh.vertices().end(); it++) {
        bbMin.minimize(quadMesh.point(it));
        bbMax.maximize(quadMesh.point(it));
    }

    return std::array{
        std::array{bbMin[0], bbMax[0]},
        std::array{bbMin[1], bbMax[1]},
        std::array{bbMin[2], bbMax[2]}
    };
}
