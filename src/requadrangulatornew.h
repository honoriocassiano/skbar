//
// Created by cassiano on 07/10/2020.
//

#ifndef SKBAR_REQUADRANGULATORNEW_H
#define SKBAR_REQUADRANGULATORNEW_H

#include "editablemesh.h"

namespace skbar {

class RequadrangulatorNew {

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
    static void RequadrangulateAll(EditableMesh &editableMesh);

private:
    static std::map<int, std::vector<InOutSKVIndex>> FindAffectedPatches(const EditableMesh &editableMesh);

    static void
    RequadrangulatePatches(EditableMesh &editableMesh,
                           const std::map<int, std::vector<InOutSKVIndex>> &inOutsByPatchMap);

    /**
     * Add a new vertex to quadmesh
     * @param position position of new vertex
     * @return index of new vertex
     */
    static int AddQuadVertex(QuadMesh &mesh, const Vec3f &position);

    /**
     * Split the quad mesh according to the sketch
     *
     * @param mesh
     * @return a map indexed by the new vertices whose values says if the vertex is on the border
     */
    static std::map<int, bool> CutQuadWithSketch(EditableMesh &mesh);

    static TraceData TracePolygons(skbar::EditableMesh &editableMesh,
                                   const std::map<int, std::vector<InOutSKVIndex>> &inOutsByPatchMap);

    static int AddSketchVertexToQuadMesh(QuadMesh &mesh, const SketchVertex &sketchVertex);

    static void CreateNewFacesOnQuadMesh(QuadMesh &mesh, int patch, const QuadMesh &newPatch,
                                         const std::map<int, int> &newPatchToQuadMesh);

    /**
     * Get all half-edges that cross the sketch in some vertex
     *
     * @return a map that maps the edge index on quadmesh to the inOut index
     */
    static std::map<int, std::size_t> FindEdgesToCheck(const EditableMesh &editableMesh, int patch,
                                                       const std::vector<InOutSKVIndex> &inOuts);


};
}

#endif //SKBAR_REQUADRANGULATORNEW_H
