#include "optrimesh.h"

#include "opquadmesh.h"

skbar::OPTriMesh::OPTriMesh() {

}

skbar::OPTriMesh::~OPTriMesh() {

}

bool Load(skbar::OPTriMesh::TriMeshImpl &mesh, const skbar::OPQuadMesh &baseMesh) {

    auto tempTriMesh = baseMesh.Get();

    mesh.assign(tempTriMesh);

    // Copy data from quad mesh
    for (auto fi : mesh.faces()) {
        mesh.data(fi).quadFaceData.id = tempTriMesh.data(fi).quadFaceData.id;
    }

    return true;
}

bool skbar::OPTriMesh::Load(const skbar::QuadMesh &mesh) {

    return Load(dynamic_cast<const OPQuadMesh&>(mesh));
}

std::size_t skbar::OPTriMesh::GetQuadFaceId(std::size_t triangleId) const {

}
