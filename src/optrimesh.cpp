#include "optrimesh.h"

#include "opquadmesh.h"
#include "opmeshrenderer.h"

skbar::OPTriMesh::OPTriMesh() {

}

skbar::OPTriMesh::~OPTriMesh() {

}

bool OPLoad(skbar::OPTriMesh::TriMeshImpl &mesh, const skbar::OPQuadMesh &baseMesh) {

    auto tempTriMesh = baseMesh.GetTriangulated().Get();

    mesh.assign(tempTriMesh);

    // Copy data from quad mesh
    for (auto fi : mesh.faces()) {
        mesh.data(fi).quadFaceData.id = tempTriMesh.data(fi).quadFaceData.id;
    }

    return true;
}

bool skbar::OPTriMesh::Load(const skbar::QuadMesh &qmesh) {

    return OPLoad(mesh, dynamic_cast<const OPQuadMesh &>(qmesh));
}

std::size_t skbar::OPTriMesh::GetQuadFaceId(std::size_t triangleId) const {

}

void skbar::OPTriMesh::Render() const {
    OPMeshRenderer::Render(mesh);
}
