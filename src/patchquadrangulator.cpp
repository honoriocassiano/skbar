//
// Created by cassiano on 03/05/2020.
//

#include <utils/debug.h>
#include <algorithm>
#include <numeric>

#include "patchquadrangulator.h"

#include "patchgen/decl.hh"

const double skbar::PatchQuadrangulator::LAPLACE_CONSTRAINT_WEIGHT = 10 * 1000;

patchgen::PatchParam skbar::PatchQuadrangulator::ComputeTopology(const Eigen::VectorXi &patchSides, QuadMesh &mesh) {
    patchgen::PatchParam param;

    patchgen::generate_topology<QuadMesh>(patchSides, param, mesh);

    return param;
}

Eigen::SparseMatrix<double> skbar::PatchQuadrangulator::GetLaplacianMatrix(const skbar::QuadMesh &mesh,
                                                                           const patchgen::PatchParam &param) {

    typedef Eigen::Triplet<double> Triplet;
    typedef Eigen::SparseMatrix<double> Matrix;

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

Eigen::Matrix<double, -1, 3> skbar::PatchQuadrangulator::GetRightSide(const QuadMesh &mesh,
                                                                      const patchgen::PatchParam &param) {

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

void skbar::PatchQuadrangulator::SetLaplacianPositions(QuadMesh &patch,
                                                       const std::vector<OpenMesh::Vec3d> &positions) {

    OpenMesh::SmartVertexHandle currentVertex;
    OpenMesh::SmartHalfedgeHandle currentEdge;

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

    // Loop over inversed position vector because boundary halfedges are oriented CLOCKWISE (IMPORTANT!)
    for (auto it = positions.rbegin(); it != positions.rend(); it++) {
        patch.data(currentVertex).laplacian.position = *it;
        patch.data(currentVertex).laplacian.fixed = true;

        currentEdge = currentEdge.next();
        currentVertex = currentEdge.to();
    }
}