#include <igl/readOBJ.h>
#include <igl/parallel_for.h>

#include <cstdio>

//#include "findseparatrices.h"

std::vector<std::vector<int>> ComputeVertexVertexAdjacency(
		const Eigen::MatrixXd& V, const Eigen::MatrixXi& F) {

	std::vector<std::vector<int>> adj;

	adj.resize(V.rows());

	for (int i = 0; i < F.rows(); ++i) {
		for (int j = 0; j < 4; ++j) {
			int v1 = F(i, j);
			int v2 = F(i, (j + 1) % 4);

			adj.at(v1).push_back(v2);
			adj.at(v2).push_back(v1);

		}
	}

	for (int i = 0; i < adj.size(); ++i) {
		std::sort(adj[i].begin(), adj[i].end());

		adj[i].erase(std::unique(adj[i].begin(), adj[i].end()), adj[i].end());
	}

	return adj;
}

std::vector<std::vector<int>> ComputeVertexQuadAdjacency(
		const Eigen::MatrixXd& V, const Eigen::MatrixXi& F) {

	std::vector<std::vector<int>> adj;

	adj.resize(V.rows());

	for (int i = 0; i < F.rows(); ++i) {
		int v1 = F(i, 0);
		int v2 = F(i, 1);
		int v3 = F(i, 2);
		int v4 = F(i, 3);

		adj.at(v1).push_back(i);
		adj.at(v2).push_back(i);
		adj.at(v3).push_back(i);
		adj.at(v4).push_back(i);
	}

	return adj;
}

void ComputeQuadQuadAdjacency(const Eigen::MatrixXd& V,
		const Eigen::MatrixXi& F, Eigen::MatrixXi& QQ, Eigen::MatrixXi& QE) {

	auto vqAdj = ComputeVertexQuadAdjacency(V, F);

	QQ = F.Constant(F.rows(), 4, -1);
	QE = F.Constant(F.rows(), 4, -1);

	igl::parallel_for(F.rows(), [&](int f) {
		for (int i = 0; i < 4; i++) {
			int vi = F(f, i);
			int vin = F(f, (i + 1) % 4);

			// Iterate over neighbor quads
			for(int j = 0; j < vqAdj[vi].size(); j++) {

				int fn = vqAdj[vi][j];

				if(fn != f) {
					// Check if one of the edge share the same edge
					for(int kn = 0; kn < 4; kn++) {
						if(F(fn, kn) == vin) {
							QQ(f,i) = fn;
							QE(f, i) = kn;

							break;
						}
					}

//					if (F(fn, 0) == vin || F(fn, 1) == vin
//							|| F(fn, 2) == vin || F(fn, 3) == vin) {
//						QQ(f,i) = fn;
//
////						QE(f, /)
//
//						break;
//					}
				}
			}
		}
	});
}

//std::vector<std::vector<int>> ComputeVertexQuadAdjacency(
//		const Eigen::MatrixXd& V, const Eigen::MatrixXi& F, const std::vector<std::vector<int>>& adjVQ) {
//
//	std::vector<std::vector<int>> adj;
//
//	adj.resize(F.rows());
//
//	for(int i = 0; i < F.rows(); ++i) {
//
//	}
//
//	return adj;
//}

int main(int argc, char **argv) {

	Eigen::MatrixXd V;
	Eigen::MatrixXi F;

	// Load a mesh in OFF format
//	igl::readOBJ("./models/3holes_quad.obj", V, F);
	igl::readOBJ("./models/plane.obj", V, F);

//    std::cout << F << std::endl;

//    skbar::FindSeparatrices f;
//
//    f.Find(V, F);

	Eigen::VectorXi adjCount;
	std::vector<int> singularities;

//	singularities.resize(V.rows());

	adjCount.resize(V.rows(), 1);
	adjCount.setZero(V.rows(), 1);

	for (int i = 0; i < F.rows(); ++i) {
		for (int j = 0; j < F.row(i).cols(); ++j) {
			int v = F(i, j);

			adjCount(v) += 1;
		}
	}

	for (int i = 0; i < adjCount.rows(); ++i) {
		if (adjCount(i) == 4) {
			singularities.push_back(i);
		}
	}

//	std::cout << singularities << std::endl;

// for (int v : singularities) {
// 	std::printf("%d\n", v);
// }

// Compute vertices adjacency
//	auto adj = ComputeVertexVertexAdjacency(V, F);
//	auto adj = ComputeVertexQuadAdjacency(V, F);
//
//		for (auto a : adj) {
//			std::printf("%ld\n", a.size());
//		}

	Eigen::MatrixXi QQ;
	Eigen::MatrixXi QE;

	ComputeQuadQuadAdjacency(V, F, QQ, QE);

	std::cout << QQ << "\n" << std::endl;
	std::cout << QE << std::endl;

	return 0;
}
