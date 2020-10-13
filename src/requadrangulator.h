//
// Created by cassiano on 09/09/2020.
//

#ifndef SKBAR_REQUADRANGULATOR_H
#define SKBAR_REQUADRANGULATOR_H

#include "editablemesh.h"

namespace skbar {

// TODO Set methods in this class as static
class Requadrangulator {

    struct InOutSKVIndex {
        int in;
        int out;
    };

    struct PolygonData {
        // Num of edges on each size of the polygon
        std::vector<int> edgesBySide;

        // Positions of each vertex on border
        std::vector<Vec2f> borderVertexPositions;

        // Indices of vertices on border
        std::vector<int> borderVertices;
    };

    struct TraceData {
        // key = patch
        std::multimap<int, PolygonData> polygonsData;

        // Added vertices according to sketch
        std::vector<int> sketchVertexOnMesh;

        // Edges that must be splitter according to sketch
        std::vector<bool> mustSplitEdge;
    };

public:
    explicit Requadrangulator(EditableMesh *mesh);

    virtual ~Requadrangulator() = default;

    void RequadrangulateAll();

private:

    /**
     * Return a map indexed by patches that maps to the ins and outs sketch vertex indices
     */
    std::map<int, std::vector<InOutSKVIndex>> FindAffectedPatches() const;

    void RequadrangulatePatch(int patch, const std::vector<InOutSKVIndex> &inOuts);

    void RequadrangulatePatchWithoutHole(int patch, const std::vector<InOutSKVIndex> &inOuts);

    std::vector<std::vector<Vec2f>> FindSidesOfPatch(const SketchVertex &firstSV, const SketchVertex &lastSV) const;

    static std::vector<int> FindCommonPatches(const SketchVertex &sv0, const SketchVertex &sv1);

    static TraceData TracePolygons(skbar::EditableMesh &editableMesh,
                                   const std::map<int,
                                           std::vector<skbar::Requadrangulator::InOutSKVIndex>> &inOutsByPatchMap);

    static std::pair<int, int>
    FindQuadTriangles(const QuadMesh &quadMesh, const TriMesh &triMesh, int quadId);

    /**
     * Get all half-edges that cross the sketch in some vertex
     *
     * @return a map that maps the edge index on quadmesh to the inOut index
     */
    static std::map<int, std::size_t> FindEdgesToCheck(const EditableMesh &editableMesh, int patch,
                                                       const std::vector<InOutSKVIndex> &inOuts);

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

    /**
     * Split the quad mesh according to the sketch
     *
     * @param mesh
     * @return a map indexed by the new vertices whose values says if the vertex is on the border
     */
    static std::map<int, bool> CutQuadWithSketch(EditableMesh &mesh);

//    static PolygonData
//    WalkOnBorder(const EditableMesh &mesh, int patch, int heIndex, const std::vector<InOutSKVIndex> &inOutsOnPatch,
//                 const std::map<int, std::size_t> &edgesToCheck, std::map<int, bool> &halfEdgesChecked);

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

    static int AddQuadFace(QuadMesh &mesh, const std::vector<int> &verticesId);

    // TODO Remove this function
    [[deprecated]]
    static void DeletePatchFaces(QuadMesh &mesh, int patch);

    static void DeleteAllPatchFaces(QuadMesh &mesh, std::set<int> patches);

    [[deprecated]]
    static int AddSketchVertexToQuadMesh(QuadMesh &mesh, const SketchVertex &sketchVertex);

    static int AddSketchVertexToQuadMesh2(QuadMesh &mesh, const SketchVertex &sketchVertex);

    static void SplitQuadEdges(QuadMesh &mesh, const Sketch &sketch, const std::vector<int> &sketchVertexOnMesh,
                               const std::vector<bool> &mustSplitEdge
    );

    /**
     * Map and add the new patch vertices to quad mesh
     * @param newPatch the new patch
     * @return a map that maps the vertices on newPatch to vertices on quadmesh
     */
    static std::map<int, int>
    MapNewPatchToQuadMesh(QuadMesh &mesh, const QuadMesh &newPatch, const std::vector<int> &borderVertices);

    static void CreateNewFacesOnQuadMesh(QuadMesh &mesh, int patch, const QuadMesh &newPatch,
                                         const std::map<int, int> &newPatchToQuadMesh);

    static void
    RequadrangulatePatches(EditableMesh &editableMesh,
                           const std::map<int, std::vector<InOutSKVIndex>> &inOutsByPatchMap);

private:
    EditableMesh *editableMesh;
};
}

#endif //SKBAR_REQUADRANGULATOR_H
