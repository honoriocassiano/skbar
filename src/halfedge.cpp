/*
 * halfedgeiterator.cpp
 *
 *  Created on: 2 de abr de 2020
 *      Author: cassiano
 */

#include "halfedge.h"

namespace skbar {

HalfEdge::Iterator::Iterator(const Eigen::MatrixXd& _vertices,
		const Eigen::MatrixXi& _faces, const Eigen::MatrixXi& _QQ,
		const Eigen::MatrixXi& _QE, int _face, int _edge, bool _reverse) :
		vertices(_vertices), faces(_faces), QQ(_QQ), QE(_QE), face(_face), edge(
				_edge), reverse(_reverse) {
}

HalfEdge::HalfEdge(const Eigen::MatrixXd& _vertices,
		const Eigen::MatrixXi& _faces) :
		vertices(_vertices), faces(_faces) {

}

HalfEdge::~HalfEdge() {

}

HalfEdge::Iterator HalfEdge::Begin(int face, int edge, bool reverse) {
	return Iterator(vertices, faces, QQ, QE, face, edge, reverse);
}

Eigen::VectorXd HalfEdge::Iterator::V() const {
	return vertices.row(Vi());
}

int HalfEdge::Iterator::Vi() const {

	assert(face >= 0);
	assert(face < faces.rows());
	assert(edge >= 0);
	assert(edge <= 3);

	if (!reverse) {
		return (faces)(face, edge);
	} else {
		return (faces)(face, (edge + 1) % 4);
	}
}

void HalfEdge::Iterator::FlipF() {
	if (IsBorder()) {
		return;
	}

	int fin = (QQ)(face, edge);
	int ein = (QE)(face, edge);

	face = fin;
	edge = ein;
	reverse = !reverse;
}

void HalfEdge::Iterator::FlipE() {
	if (!reverse) {
		edge = (edge + 3) % 4; // edge-1
	} else {
		edge = (edge + 1) % 4;
	}

	reverse = !reverse;
}

int HalfEdge::Iterator::FFlip() const {
	return QQ(face, edge);
}

int HalfEdge::Iterator::VFlip() const {
	assert(face >= 0);
	assert(face < faces.rows());
	assert(edge >= 0);
	assert(edge <= 3);

	// Flip condition before return
	if (reverse) {
		return (faces)(face, edge);
	} else {
		return (faces)(face, (edge + 1) % 4);
	}
}

int HalfEdge::Iterator::EFlip() const {
	if (!reverse) {
		return (edge + 3) % 4; // edge-1
	} else {
		return (edge + 1) % 4;
	}
}

bool HalfEdge::Iterator::operator ==(Iterator& p2) const {
	return ((face == p2.face) && (edge == p2.edge) && (reverse == p2.reverse)
			&& (faces == p2.faces) && (QQ == p2.QQ) && (QE == p2.QE));
}

} /* namespace skbar */

