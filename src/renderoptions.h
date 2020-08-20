//
// Created by cassiano on 20/08/2020.
//

#ifndef SKBAR_RENDEROPTIONS_H
#define SKBAR_RENDEROPTIONS_H

namespace skbar::renderer {

struct TriMeshOptions {
    bool drawEdges;

    TriMeshOptions() : drawEdges(true) {}
    virtual ~TriMeshOptions() = default;
};

struct QuadMeshOptions {
    bool drawEdges;
    bool drawPatches;

    QuadMeshOptions() : drawEdges(true), drawPatches(false) {};
    virtual ~QuadMeshOptions() = default;
};

struct SketchOptions {

    float lineColor[3];
    float lineWidth;

    SketchOptions() : lineColor{0, 1, 0}, lineWidth(2.5) {}
    virtual ~SketchOptions() = default;

};

struct Options {

    enum class Mesh { TRI, QUAD } mesh;

    bool drawSketch;

    TriMeshOptions triOptions;
    QuadMeshOptions quadOptions;
    SketchOptions skOptions;

    Options() : mesh(Mesh::QUAD), drawSketch(true) {}
    virtual ~Options() = default;
};
}

#endif //SKBAR_RENDEROPTIONS_H
