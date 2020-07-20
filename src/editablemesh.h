#ifndef EDITABLE_MESH_H
#define EDITABLE_MESH_H

#include "meshtypes.h"
#include "commontypes.h"
#include "quadmesh.h"
#include "trimesh.h"
#include "sketch.h"

namespace skbar {

class EditableMesh {

public:

    EditableMesh();

    virtual ~EditableMesh();

    bool Load(const std::string &filename);

    BBox GetBBox() const;

    void Render() const;

    Intersection<int, Vec3f> GetClosestIntersection(const Line<Vec3f> &ray) const;

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

    inline Sketch &GetSketch() {
        return *sketch;
    }

    const inline Sketch &GetSketch() const {
        return *sketch;
    }

private:

    // Triangulated mesh
    TriMesh *triMesh;

    // Quad mesh
    QuadMesh *quadMesh;

    Sketch *sketch;
};

}

#endif
