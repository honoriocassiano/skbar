#include "opquadmesh.h"

#include "utils/debug.h"

#include <OpenMesh/Core/IO/MeshIO.hh>

skbar::OPQuadMesh::OPQuadMesh() {

}

skbar::OPQuadMesh::OPQuadMesh(const skbar::OPQuadMesh::QuadMeshImpl &_mesh) : mesh(_mesh) {

}

skbar::OPQuadMesh::~OPQuadMesh() {

}

bool skbar::OPQuadMesh::Load(const std::string &filename) {

    if (!OpenMesh::IO::read_mesh(mesh, filename.c_str())) {
        Error("Cannot read file %s", filename.c_str());

        return false;
    }

    Log("%lu", mesh.n_faces());

    // Set data of current quad face id
    for (auto fi : mesh.faces()) {
        mesh.data(fi).quadFaceData.id = fi.idx();
    }

    return true;
}

void skbar::OPQuadMesh::FindPatches() {

}

skbar::OPQuadMesh skbar::OPQuadMesh::GetTriangulated() const {
    auto temp = mesh;

    temp.triangulate();

    return OPQuadMesh(temp);
}

std::size_t skbar::OPQuadMesh::GetNumPatches() const {

}

skbar::QuadMesh *skbar::OPQuadMesh::GetPatch(std::size_t patchId) {

}

void skbar::OPQuadMesh::ReplacePatch(size_t patchId, const skbar::QuadMesh &patch) {

}

skbar::BBox skbar::OPQuadMesh::GetBBox() const {

}
