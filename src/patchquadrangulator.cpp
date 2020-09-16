//
// Created by cassiano on 03/05/2020.
//

#include <utils/debug.h>
#include <algorithm>
#include <numeric>

#include "patchquadrangulator.h"

#include "patchgen/decl.hh"

const double skbar::PatchQuadrangulator::LAPLACE_CONSTRAINT_WEIGHT = 10 * 1000;

skbar::OPQuadMesh
skbar::PatchQuadrangulator::Quadrangulate(const Eigen::VectorXi &patchSides,
                                          const std::vector<OpenMesh::Vec3d> &positions,
                                          bool positionsIsClockwise) {

    OPQuadMesh newPolygon;
    auto &newMesh = newPolygon.Get();

    const auto param = PatchQuadrangulator::ComputeTopology(patchSides, newPolygon);

    PatchQuadrangulator::SetLaplacianPositions(newPolygon, positions, positionsIsClockwise);

    const auto L = PatchQuadrangulator::GetLaplacianMatrix(newPolygon, param);
    const auto b = PatchQuadrangulator::GetRightSide(newPolygon, param);

    auto solver = new Eigen::SimplicialCholesky<Eigen::SparseMatrix<double>>(L.transpose() * L);

    Eigen::Matrix<double, -1, 3> x = solver->solve(L.transpose() * b);

    for (const auto &it : newMesh.vertices()) {

        const auto pos = OpenMesh::Vec3f(x(it.idx(), 0), x(it.idx(), 1), x(it.idx(), 2));

        newMesh.set_point(it, pos);
    }

    return newPolygon;
}

patchgen::PatchParam
skbar::PatchQuadrangulator::ComputeTopology(const Eigen::VectorXi &patchSides, skbar::OPQuadMesh &mesh) {
    patchgen::PatchParam param;

    patchgen::generate_topology<OPQuadMesh::QuadMeshImpl>(patchSides, param, mesh.Get());

    return param;
}

Eigen::SparseMatrix<double> skbar::PatchQuadrangulator::GetLaplacianMatrix(const skbar::OPQuadMesh &meshBase,
                                                                           const patchgen::PatchParam &param) {

    typedef Eigen::Triplet<double> Triplet;
    typedef Eigen::SparseMatrix<double> Matrix;

    auto mesh = meshBase.Get();

    Matrix laplacianMatrix;

    auto nVertices = mesh.n_vertices();
    auto nControlPoints = param.l.sum();

    std::vector<Triplet> triplets;
    std::size_t indexFixedVertex = 0;

    laplacianMatrix.setZero();

    // Default laplacian matrix
    for (auto vertex : mesh.vertices()) {
        auto vId = vertex.idx();

        for (const auto &vn: vertex.vertices()) {
//            laplacianMatrix(i, vn.idx()) = -1;
            triplets.emplace_back(vId, vn.idx(), -1.0);
        }

        triplets.emplace_back(vId, vId, vertex.valence());

        if (mesh.data(vertex).laplacian.fixed) {
            triplets.emplace_back(nVertices + indexFixedVertex, vId, LAPLACE_CONSTRAINT_WEIGHT);

            indexFixedVertex++;
        }
    }

    laplacianMatrix.resize(nVertices + nControlPoints, nVertices);
    laplacianMatrix.setFromTriplets(triplets.begin(), triplets.end());

    return laplacianMatrix;
}

Eigen::Matrix<double, -1, 3> skbar::PatchQuadrangulator::GetRightSide(const skbar::OPQuadMesh &baseMesh,
                                                                      const patchgen::PatchParam &param) {

    auto mesh = baseMesh.Get();

    auto nv = mesh.n_vertices();
    auto fixedVertexIndex = 0;

    Eigen::Matrix<double, -1, 3> b(nv + param.l.sum(), 3);

    b.setZero();

    for (auto v : mesh.vertices()) {

        auto laplacianData = mesh.data(v).laplacian;

        if (laplacianData.fixed) {

            b(nv + fixedVertexIndex, 0) = laplacianData.position[0] * LAPLACE_CONSTRAINT_WEIGHT;
            b(nv + fixedVertexIndex, 1) = laplacianData.position[1] * LAPLACE_CONSTRAINT_WEIGHT;
            b(nv + fixedVertexIndex, 2) = laplacianData.position[2] * LAPLACE_CONSTRAINT_WEIGHT;

            fixedVertexIndex++;
        }
    }

    return b;
}

void skbar::PatchQuadrangulator::SetLaplacianPositions(skbar::OPQuadMesh &basePatch,
                                                       const std::vector<OpenMesh::Vec3d> &positions,
                                                       bool positionsIsClockwise) {

    OpenMesh::SmartVertexHandle currentVertex;
    OpenMesh::SmartHalfedgeHandle currentEdge;

    auto &patch = basePatch.Get();

    // Find first corner
    for (auto v : patch.vertices()) {
        if (patch.data(v).patchgen.corner_index == 0) {
            for (auto h : v.outgoing_halfedges()) {

                if (h.is_boundary()) {
                    currentEdge = patch.halfedge_handle(v);
                    break;
                }
            }
            break;
        }
    }

    currentVertex = currentEdge.to();

    // TODO Use DRY principle here
    if (positionsIsClockwise) {
        for (const auto &position : positions) {

            patch.data(currentVertex).laplacian.position = position;
            patch.data(currentVertex).laplacian.fixed = true;

            currentEdge = currentEdge.next();
            currentVertex = currentEdge.to();
        }
    } else {
        for (auto it = positions.rbegin(); it != positions.rend(); it++) {

            patch.data(currentVertex).laplacian.position = *it;
            patch.data(currentVertex).laplacian.fixed = true;

            currentEdge = currentEdge.next();
            currentVertex = currentEdge.to();
        }
    }
}
