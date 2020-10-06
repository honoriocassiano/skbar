#include "opquadmesh.h"

#include "utils/debug.h"
#include "opmeshrenderer.h"
#include "patchtracer.h"

#include <OpenMesh/Core/IO/MeshIO.hh>

skbar::OPQuadMesh::OPQuadMesh(const skbar::OPQuadMesh::QuadMeshImpl &_mesh) : mesh(_mesh) {

}

bool skbar::OPQuadMesh::Load(const std::string &filename) {

    if (!OpenMesh::IO::read_mesh(mesh, filename.c_str())) {
        Error("Cannot read file %s", filename.c_str());

        return false;
    }

    // Set data of current quad face id
    for (auto fi : mesh.faces()) {
        mesh.data(fi).quadFaceData.id = fi.idx();
    }

    for (auto ei : mesh.edges()) {
        mesh.data(ei).quadEdgeData.id = ei.idx();
    }

    PatchTracer::Trace(*this);

    mesh.update_normals();

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
    return patchFacesMap.size();
}

void skbar::OPQuadMesh::ReplacePatch(size_t patchId, const skbar::QuadMesh &patch) {

}

skbar::BBox skbar::OPQuadMesh::GetBBox() const {

}

bool skbar::OPQuadMesh::Save(const std::string &filename) const {

    if (!OpenMesh::IO::write_mesh(mesh, filename)) {
        Error("Cannot write file %s", filename.c_str());

        return false;
    }

    return true;
}

std::optional<std::vector<std::vector<int>>> skbar::OPQuadMesh::GetPatch(int patch) const {

    std::optional<std::vector<std::vector<int>>> result;

    if (patchFacesMap.find(patch) != patchFacesMap.end()) {
        result = patchFacesMap.at(patch);
    }

    return result;
}

std::optional<skbar::Vec2f> skbar::OPQuadMesh::GetParametricPosition(int vId, int patch) const {

    std::optional<skbar::Vec2f> result;

    const auto vData = mesh.data(OpenMesh::VertexHandle(vId)).quadVertexData.patchParametrizations;

    if (vData.find(patch) != vData.end()) {
        result = vData.at(patch);
    }

    return result;
}
