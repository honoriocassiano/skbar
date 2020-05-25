//
// Created by cassiano on 26/04/2020.
//

#ifndef SKBAR_MESHTYPES_H
#define SKBAR_MESHTYPES_H

#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <PatchVertexTag.hh>

namespace skbar {

    struct Patchgen {
        int corner_index;
        patchgen::PatchVertexTag tag;
    };

// define traits
    struct MyTraits : public OpenMesh::DefaultTraits {
        // use double valued coordinates
        typedef OpenMesh::Vec3d Point;
        // use vertex normals and vertex colors
        VertexAttributes(OpenMesh::Attributes::Normal |
                         OpenMesh::Attributes::Color);
        // store the previous halfedge
        HalfedgeAttributes(OpenMesh::Attributes::PrevHalfedge);
        // use face normals
        FaceAttributes(OpenMesh::Attributes::Normal);
        // store a face handle for each vertex
//    FaceTraits
//    {
//        typename Base::Refs::FaceHandle my_face_handle;
//    };
        VertexTraits
        {
            Patchgen patchgen;
        };
    };
// Select mesh type (TriMesh) and kernel (ArrayKernel)
// and define my personal mesh type (MyMesh)
    typedef OpenMesh::TriMesh_ArrayKernelT<MyTraits> TriMesh;
    typedef OpenMesh::PolyMesh_ArrayKernelT<MyTraits> QuadMesh;
}

#endif //SKBAR_MESHTYPES_H
