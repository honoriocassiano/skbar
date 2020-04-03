/*
 * adjacencyutils.cpp
 *
 *  Created on: 2 de abr de 2020
 *      Author: cassiano
 */

#include "adjacencyutils.h"

#include <igl/parallel_for.h>

namespace skbar {

AdjacencyUtils::AdjacencyUtils() {
	// TODO Auto-generated constructor stub

}

AdjacencyUtils::~AdjacencyUtils() {
	// TODO Auto-generated destructor stub
}

std::vector<std::vector<int> > AdjacencyUtils::ComputeVertexQuadAdjacency(
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

void AdjacencyUtils::ComputeQuadQuadAdjacency(const Eigen::MatrixXd& V,
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

} /* namespace skbar */
