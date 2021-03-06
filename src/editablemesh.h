#ifndef EDITABLE_MESH_H
#define EDITABLE_MESH_H

#include "meshtypes.h"
#include "commontypes.h"
#include "quadmesh.h"
#include "trimesh.h"

namespace skbar {

class EditableMesh {

public:

    EditableMesh();

    virtual ~EditableMesh();

    bool Load(const std::string &filename);

    BBox GetBBox() const;

    void Render() const;

    inline QuadMesh &GetQuad() {
        return *quadMesh;
    }

    inline const QuadMesh &GetQuad() const {
        return *quadMesh;
    }

    inline TriMesh &GetTri() {
        return *triMesh;
    }

    const inline TriMesh &GetTri() const {
        return *triMesh;
    }

private:

    // Triangulated mesh
    TriMesh *triMesh;

    // Quad mesh
    QuadMesh *quadMesh;
};

}

#endif
