/*
 * adjacencyutils.h
 *
 *  Created on: 2 de abr de 2020
 *      Author: cassiano
 */

#ifndef SRC_ADJACENCYUTILS_H_
#define SRC_ADJACENCYUTILS_H_

#include <vector>

#include <Eigen/Core>

namespace skbar {

class AdjacencyUtils {
private:
	AdjacencyUtils();
	virtual ~AdjacencyUtils();

public:
	static std::vector<std::vector<int>> ComputeVertexQuadAdjacency(
			const Eigen::MatrixXd& V, const Eigen::MatrixXi& F);

	static void ComputeQuadQuadAdjacency(const Eigen::MatrixXd& V,
			const Eigen::MatrixXi& F, Eigen::MatrixXi& QQ, Eigen::MatrixXi& QE);
};

} /* namespace skbar */

#endif /* SRC_ADJACENCYUTILS_H_ */
