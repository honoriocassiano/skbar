//
// Created by cassiano on 26/04/2020.
//

#ifndef SKBAR_MESHTYPES_H
#define SKBAR_MESHTYPES_H

#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <PatchVertexTag.hh>

#include <map>
#include <array>

namespace skbar {

/**
 * Holds data used by patchgen
 */
struct Patchgen {
    // Internal use of Takayama'S algorithm
    int corner_index;
    patchgen::PatchVertexTag tag;

    int indexOnPositionVector;

    Patchgen() : corner_index(-1), tag(patchgen::PatchVertexTag::None), indexOnPositionVector(-1) {}
};

/*
 * Holds data to use in laplacian calculation
 */
struct Laplacian {
    OpenMesh::Vec3d position;
    bool fixed;

    Laplacian() : fixed(false) {}
};

struct QuadVertexData {
    std::map<int, std::array<float, 2>> patchParametrizations;
    bool isCorner;

    QuadVertexData() : isCorner(false) {}
};

/*
 * Holds data of quad mesh faces
 */
struct QuadFaceData {
    int id;
    int patchId;

    QuadFaceData() : id(-1), patchId(-1) {}
};

struct QuadEdgeData {
    bool patchEdge;
    int id;

    QuadEdgeData() : patchEdge(false), id(-1) {}
};

// Define quad mesh traits
struct QuadMeshTraits : public OpenMesh::DefaultTraits {
    // use double valued coordinates
    typedef OpenMesh::Vec3f Point;
    // use vertex normals and vertex colors
    VertexAttributes(OpenMesh::Attributes::Normal |
                     OpenMesh::Attributes::Color | OpenMesh::Attributes::Status);
    // store the previous halfedge
    HalfedgeAttributes(OpenMesh::Attributes::PrevHalfedge | OpenMesh::Attributes::Status);
    // use face normals
    FaceAttributes(OpenMesh::Attributes::Normal | OpenMesh::Attributes::Status);
    // store a face handle for each vertex

    FaceTraits
    {
        QuadFaceData quadFaceData;
    };

    EdgeTraits
    {
        QuadEdgeData quadEdgeData;
    };

    VertexTraits
    {
        Patchgen patchgen;
        Laplacian laplacian;
        QuadVertexData quadVertexData;
    };
};

struct TriFaceData {
    int quadFaceId;

    TriFaceData() : quadFaceId(-1) {}
};

struct TriEdgeData {
    int quadEdgeId;

    bool IsQuadEdge() const { return quadEdgeId != -1; }

    TriEdgeData() : quadEdgeId(-1) {}
};

// Define tri mesh traits
struct TriMeshTraits : public OpenMesh::DefaultTraits {
    // use double valued coordinates
    typedef OpenMesh::Vec3f Point;
    // use vertex normals and vertex colors
    VertexAttributes(OpenMesh::Attributes::Normal |
                     OpenMesh::Attributes::Color | OpenMesh::Attributes::Status);
    // store the previous halfedge
    HalfedgeAttributes(OpenMesh::Attributes::PrevHalfedge | OpenMesh::Attributes::Status);
    // use face normals
    FaceAttributes(OpenMesh::Attributes::Normal | OpenMesh::Attributes::Status);
    // store a face handle for each vertex

    FaceTraits
    {
        TriFaceData triFaceData;
    };

    EdgeTraits
    {
        TriEdgeData triEdgeData;
    };

    // VertexTraits
    // {
    // };
};
// Select mesh type (TriMesh) and kernel (ArrayKernel)
// and define my personal mesh type (MyMesh)
// typedef OpenMesh::TriMesh_ArrayKernelT<TriMeshTraits> TriMesh;
// typedef OpenMesh::TriMesh_ArrayKernelT<TriMeshTraits> TriMesh;
// typedef OpenMesh::PolyMesh_ArrayKernelT<QuadMeshTraits> QuadMesh;
}

#endif //SKBAR_MESHTYPES_H
