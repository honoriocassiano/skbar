/*
 * halfedgeiterator.cpp
 *
 *  Created on: 2 de abr de 2020
 *      Author: cassiano
 */

#include "halfedge.h"

namespace skbar {

HalfEdge::HalfEdge(const Eigen::MatrixXd& _vertices,
		const Eigen::MatrixXi& _faces) :
		vertices(_vertices), faces(_faces) {

}

HalfEdge::~HalfEdge() {

}

HalfEdge::Iterator HalfEdge::Begin(int face, int edge, bool reverse) {
	// TODO Implement this
}

HalfEdge::Iterator::Iterator(Eigen::MatrixXd& V, Eigen::MatrixXi& F,
		Eigen::MatrixXi& QQ, Eigen::MatrixXi& QE, int face, int edge,
		bool reverse) {
	// TODO Implement this
}

Eigen::VectorXi HalfEdge::Iterator::V() const {
	// TODO Implement this
}

int HalfEdge::Iterator::Vi() const {
	// TODO Implement this
}

void HalfEdge::Iterator::FlipF() {
	// TODO Implement this
}

void HalfEdge::Iterator::FlipE() {
	// TODO Implement this
}

void HalfEdge::Iterator::FlipV() {
	// TODO Implement this
}

int HalfEdge::Iterator::FFlip() const {
	// TODO Implement this
}

int HalfEdge::Iterator::VFlip() const {
	// TODO Implement this
}

int HalfEdge::Iterator::EFlip() const {
	// TODO Implement this
}

} /* namespace skbar */

