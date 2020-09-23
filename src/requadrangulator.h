//
// Created by cassiano on 09/09/2020.
//

#ifndef SKBAR_REQUADRANGULATOR_H
#define SKBAR_REQUADRANGULATOR_H

#include "editablemesh.h"

namespace skbar {

// TODO Set methods in this class as static
class Requadrangulator {

public:
    explicit Requadrangulator(EditableMesh *mesh);

    virtual ~Requadrangulator() = default;

    void RequadrangulateAll();

private:

    /**
     * Return a map indexed by patches that maps to the ins and outs sketch vertex indices
     */
    std::map<int, std::vector<std::tuple<int, int>>> FindAffectedPatches() const;

    void RequadrangulatePatch(int patch, const std::vector<std::tuple<int, int>> &inOuts);

    void RequadrangulatePatchWithoutHole(int patch, const std::vector<std::tuple<int, int>> &inOuts);

    std::vector<std::vector<Vec2f>> FindSidesOfPatch(const SketchVertex &firstSV, const SketchVertex &lastSV) const;

    static std::vector<int> FindCommonPatches(const SketchVertex &sv0, const SketchVertex &sv1);

    /***
     * Insert a new vertex and split the edge
     * @param edgeId id of edge
     * @param position position of new vertex
     * @return index of new vertex
     */
    int SplitQuadEdge(unsigned int edgeId, const Vec3f &position);

    /**
     * Add a new vertex to quadmesh
     * @param position position of new vertex
     * @return index of new vertex
     */
    int AddQuadVertex(const Vec3f &position);

    int AddQuadFace(QuadMesh &mesh, const std::vector<int> &verticesId);

    void DeletePatchFaces(int patch);

    int AddSketchVertexToQuadMesh(const SketchVertex &sketchVertex);

    /**
     * Map and add the new patch vertices to quad mesh
     * @param newPatch the new patch
     * @return a map that maps the vertices on newPatch to vertices on quadmesh
     */
    std::map<OpenMesh::SmartVertexHandle, OpenMesh::SmartVertexHandle>
    MapNewPatchToQuadMesh(QuadMesh &mesh, const QuadMesh &newPatch, const std::vector<int> &borderVertices);

    void CreateNewFacesOnQuadMesh(QuadMesh &mesh, int patch, const QuadMesh &newPatch,
                                  const std::map<OpenMesh::SmartVertexHandle, OpenMesh::SmartVertexHandle> &newPatchToQuadMesh);

private:
    EditableMesh *editableMesh;
};
}

#endif //SKBAR_REQUADRANGULATOR_H
