// #include <OpenMesh/Core/IO/MeshIO.hh>
// #include <utils/debug.h>
// #include <Eigen/SparseCholesky>
//
// #include "meshtypes.h"
// #include "patchtracer.h"
// #include "patchquadrangulator.h"
//
// #include "example.h"

#include "sdlapplication.h"

#include "editablemesh.h"
#include "utils/debug.h"
#include "vector.h"
#include "quadmesh.h"
#include "trimesh.h"

#include "intersectionutils.h"
#include "opquadmesh.h"
#include "patchquadrangulator.h"
#include "utils/openmesh.h"
#include "constants.h"

using namespace skbar;

using VSketch = std::vector<Vec3f>;

struct PolygonInfo {
    std::map<std::pair<int, int>, OpenMesh::VertexHandle> parametricVertices;
    int u_length;
    int v_length;
};

VSketch GetSketch() {

    VSketch result;

    result.push_back(Vec3f{0, 0.5, 0.0});
    result.push_back(Vec3f{0.2, 0.6, 0.0});
    result.push_back(Vec3f{0.5, 0.43, 0.0});
    result.push_back(Vec3f{0.68, 0.51, 0.0});
    result.push_back(Vec3f{0.72, 0.37, 0.0});
    result.push_back(Vec3f{0.65, 0.2, 0.0});
    result.push_back(Vec3f{0.7, 0, 0.0});

    return result;
}

PolygonInfo GetPolygonInfo() {

    const int u_length = 2;
    const int v_length = 2;

    std::map<std::pair<int, int>, OpenMesh::VertexHandle> parametricVertices;

    parametricVertices.insert_or_assign({0, 0}, OpenMesh::VertexHandle(0));
    parametricVertices.insert_or_assign({1, 0}, OpenMesh::VertexHandle(1));
    parametricVertices.insert_or_assign({0, 1}, OpenMesh::VertexHandle(2));
    parametricVertices.insert_or_assign({1, 1}, OpenMesh::VertexHandle(3));

    return PolygonInfo{
            parametricVertices,
            u_length,
            v_length
    };
}

OPQuadMesh QuadrangulatePolygon1(const PolygonInfo &polygonInfo, const VSketch &sketch) {

    Eigen::VectorXi sides;
    std::vector<OpenMesh::Vec3d> positions;

    // Fill sizes
    sides.resize(5);
    sides << 1, 1, 1, 6, 1;

    {
        // Fill positions
        positions.emplace_back(1.0, 0.0, 0.0);
        positions.emplace_back(1.0, 1.0, 0.0);
        positions.emplace_back(0.0, 1.0, 0.0);

        for (const auto &vertex: sketch) {
            positions.emplace_back(vertex[0], vertex[1], vertex[2]);
        }
    }

    return PatchQuadrangulator::Quadrangulate(sides, positions, false);
}

void RestorePolygon(OPQuadMesh &oldPolygon, const PolygonInfo &polygonInfo, const OPQuadMesh &newPolygon) {
    auto &oldMesh = oldPolygon.Get();
    auto &newMesh = newPolygon.Get();

    std::map<int, int> mapNewVerticesOnOldMesh;

    for (const auto &face : newMesh.faces()) {

        std::vector<OpenMesh::VertexHandle> verticesOnOldMesh;

        for (const auto &vertex : face.vertices()) {
            const bool fixed = newMesh.data(vertex).laplacian.fixed;
            const OpenMesh::Vec3d position = newMesh.data(vertex).laplacian.position;

            const double u = position[0] * polygonInfo.u_length;
            const double v = position[1] * polygonInfo.v_length;

            const int round_u = int(round(u));
            const int round_v = int(round(v));

            if (mapNewVerticesOnOldMesh.find(vertex.idx()) != mapNewVerticesOnOldMesh.end()) {
                verticesOnOldMesh.emplace_back(mapNewVerticesOnOldMesh.at(vertex.idx()));
            } else {

                if ((((u - round_u) + (v - round_v)) < constants::epsilon)) {
                    // Vertices already on mesh
                    const auto &vertexOnOldMesh = polygonInfo.parametricVertices.at(std::make_pair(round_u, round_v));

                    // Add to face vertices on old mesh
                    verticesOnOldMesh.emplace_back(vertexOnOldMesh.idx());

                    // Update the map with the new found vertex
                    mapNewVerticesOnOldMesh.insert_or_assign(vertex.idx(), vertexOnOldMesh.idx());
                } else {
                    // TODO Unparametrize and add to Old mesh
                }
            }

            // Add the new face
            oldMesh.add_face(verticesOnOldMesh);
        }
    }
}

int main(int argc, char *argv[]) {

//    SDLApplication app("SkBaR", 1024, 768);
//
//    app.Initialize();
//
//    app.LoadMesh("/home/cassiano/Documents/workspace-cpp/skbar/models/3holes_quad.obj");
//    // app.LoadMesh("/home/cassiano/Documents/workspace-cpp/skbar/models/plane.obj");
//
//    app.Run();

    EditableMesh mesh;

    mesh.Load("../models/parametric_plane.obj");

    const auto &quadmesh = dynamic_cast<const OPQuadMesh &>(mesh.GetQuad()).Get();

    auto sketch = GetSketch();
    auto polygonInfo = GetPolygonInfo();

    OPQuadMesh newPolygon = QuadrangulatePolygon1(polygonInfo, sketch);

    return 0;
}
