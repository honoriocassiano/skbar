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

    static std::pair<int, int>
    FindQuadTriangles(const QuadMesh &quadMesh, const TriMesh &triMesh, int quadId);

    /**
     * Add new vertices to quadmesh and return a map that maps the indices in newPatch to indices in quadmesh
     *
     * @param aQuadMesh
     * @param triMesh
     * @param newPatch
     * @param patch
     * @return
     */
    static std::map<int, int>
    AddNewVerticesToQuad(QuadMesh &aQuadMesh, const TriMesh &triMesh, const QuadMesh &newPatch, int patch);

    static Vec3f UnparametrizeVertex(const QuadMesh &aQuadMesh, const TriMesh &triMesh, int patch,
                                     const Vec2f parametricPosition);

    static std::optional<Vec3f>
    GetUnparametrizedPosition(const QuadMesh &aQuadMesh, const TriMesh &triMesh, int triFaceId,
                              const skbar::Vec2f &parametricPosition);

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
    static int AddQuadVertex(QuadMesh &mesh, const Vec3f &position);

    int AddQuadFace(QuadMesh &mesh, const std::vector<int> &verticesId);

    void DeletePatchFaces(int patch);

    int AddSketchVertexToQuadMesh(const SketchVertex &sketchVertex);

    /**
     * Map and add the new patch vertices to quad mesh
     * @param newPatch the new patch
     * @return a map that maps the vertices on newPatch to vertices on quadmesh
     */
    std::map<int, int>
    MapNewPatchToQuadMesh(QuadMesh &mesh, const QuadMesh &newPatch, const std::vector<int> &borderVertices);

    void CreateNewFacesOnQuadMesh(QuadMesh &mesh, int patch, const QuadMesh &newPatch,
                                  const std::map<int, int> &newPatchToQuadMesh);

private:
    EditableMesh *editableMesh;
};
}

#endif //SKBAR_REQUADRANGULATOR_H
