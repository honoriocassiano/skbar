//
// Created by cassiano on 26/04/2020.
//

#ifndef SKBAR_MESHTYPES_H
#define SKBAR_MESHTYPES_H

#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <PatchVertexTag.hh>

namespace skbar {

    /**
     * Holds data used by patchgen
     */
    struct Patchgen {
        int corner_index;
        patchgen::PatchVertexTag tag;

        Patchgen() : corner_index(-1), tag(patchgen::PatchVertexTag::None) {}
    };

    /*
     * Holds data to use in laplacian calculation
     */
    struct Laplacian {
        OpenMesh::Vec3d position;
        bool fixed;

        Laplacian() : fixed(false) {}
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

        QuadEdgeData() : patchEdge(false) {}
    };

    // Define quad mesh traits
    struct QuadMeshTraits : public OpenMesh::DefaultTraits {
        // use double valued coordinates
        typedef OpenMesh::Vec3f Point;
        // use vertex normals and vertex colors
        VertexAttributes(OpenMesh::Attributes::Normal |
                         OpenMesh::Attributes::Color);
        // store the previous halfedge
        HalfedgeAttributes(OpenMesh::Attributes::PrevHalfedge);
        // use face normals
        FaceAttributes(OpenMesh::Attributes::Normal);
        // store a face handle for each vertex

        FaceTraits {
            QuadFaceData quadFaceData;
        };

        EdgeTraits {
            QuadEdgeData quadEdgeData;
        };

        VertexTraits
        {
            Patchgen patchgen;
            Laplacian laplacian;
        };
    };

    // Define tri mesh traits
    struct TriMeshTraits : public OpenMesh::DefaultTraits {
        // use double valued coordinates
        typedef OpenMesh::Vec3f Point;
        // use vertex normals and vertex colors
        VertexAttributes(OpenMesh::Attributes::Normal |
                         OpenMesh::Attributes::Color);
        // store the previous halfedge
        HalfedgeAttributes(OpenMesh::Attributes::PrevHalfedge);
        // use face normals
        FaceAttributes(OpenMesh::Attributes::Normal);
        // store a face handle for each vertex

        FaceTraits {
            QuadFaceData quadFaceData;
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
