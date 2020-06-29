#ifndef EDITABLE_MESH_H
#define EDITABLE_MESH_H

#include "meshtypes.h"

namespace skbar {

class EditableMesh {

public:
    EditableMesh();

    virtual ~EditableMesh();

    bool Load(const std::string &filename);

    inline QuadMesh &GetQuad() {
        return quadMesh;
    }

    inline const QuadMesh &GetQuad() const {
        return quadMesh;
    }

    inline TriMesh &GetTri() {
        return triMesh;
    }

    const inline TriMesh &GetTri() const {
        return triMesh;
    }

private:
    std::tuple<OpenMesh::Vec3f, OpenMesh::Vec3f> GetBoundingBox() const;

private:

    // Triangulated mesh
    TriMesh triMesh;

    // Quad mesh
    QuadMesh quadMesh;
};

}

#endif
