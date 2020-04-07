/*
 * halfedgeiterator.h
 *
 *  Created on: 2 de abr de 2020
 *      Author: cassiano
 */

#ifndef SRC_HALFEDGE_H_
#define SRC_HALFEDGE_H_

#include <Eigen/Core>

namespace skbar {

class HalfEdge {

public:
	class Iterator {

	public:
		Iterator(const Eigen::MatrixXd& vertices, const Eigen::MatrixXi& faces, const Eigen::MatrixXi& QQ,
				const Eigen::MatrixXi& QE, int face, int edge, bool reverse);

		/**
		 * Get current vertex
		 * @return current vertex coordinates
		 */
		Eigen::VectorXd V() const;

		/**
		 * Get current vertex index
		 * @return index of vertex on matrix V
		 */
		int Vi() const;

		/**
		 * Get current face
		 * @return index of face on matrix F
		 */
		inline int F() const {
			return face;
		}

		/**
		 * Get current edge
		 * @return index of edge on current quad
		 */
		inline int E() const {
			return edge;
		}

		/**
		 * Flip the face
		 */
		void FlipF();

		/**
		 * Flip the edge
		 */
		void FlipE();

		/**
		 * Flip the vertex
		 */
		inline void FlipV() {
			reverse = !reverse;
		}

		/**
		 * Get opposite face
		 * @return index of opposite face on matrix F
		 */
		int FFlip() const;

		/**
		 * Get opposite vertex of current edge
		 * @return index of opposite vertex on matrix V
		 */
		int VFlip() const;

		/**
		 * Get reversed edge on opposite face
		 * @return index of reversed edge on opposite face
		 */
		int EFlip() const;

		/**
		 * Check if current position has a reverse edge
		 * @return true if doesn't exist a reverse edge and false if not
		 */
		inline bool IsBorder() const {
			return QQ(face, edge) == -1;
		}

		/**
		 * Operator to compare two iterators
		 * @param p2 other iterator
		 * @return true if both iterators point to the same face, edge and vertex
		 */
		bool operator==(Iterator& p2) const;

	private:
		const Eigen::MatrixXd& vertices;
		const Eigen::MatrixXi& faces;

		const Eigen::MatrixXi& QQ;
		const Eigen::MatrixXi& QE;

		int face;
		int edge;
		bool reverse;
	};

public:
	HalfEdge(const Eigen::MatrixXd& vertices, const Eigen::MatrixXi& faces);
	virtual ~HalfEdge();

	Iterator Begin(int face, int edge, bool reverse);

private:
	Eigen::MatrixXd vertices;
	Eigen::MatrixXi faces;

	/**
	 * Matrix of quad-quad adjacency
	 *
	 * This matrix maps two quads to the index of adjacent edge on first quad
	 */
	Eigen::MatrixXi QQ;

	/**
	 * Matrix of quad-edge adjacency
	 *
	 * This matrix maps a quad and its edge to corresponding edge in adjacent quad
	 */
	Eigen::MatrixXi QE;
};

} /* namespace skbar */

#endif /* SRC_HALFEDGE_H_ */
