/*
 *  EdgeCollapse.h
 *  aloe
 *
 *  mesh simplification by incremental decimation
 *  topological operator is half edge collapse
 *  vertex list does not expand
 *  sort edges by cost
 *  select lowest cost edge (p, q) at the list beginning
 *  collapse q to p where cost of p > q
 *  removing faces around q creates a hold
 *  re-triangulate the hole reducing number of faces by two
 *  relocate removed vertex and faces to end
 *  cost of collapes a vertex <- max (1 - Na dot Nb)
 *  Na, Nb is normal of any face pair connected to the vertex
 *  keep edges on border
 *
 */

#ifndef ALO_EDGE_COLLAPSE_H
#define ALO_EDGE_COLLAPSE_H

#include "MeshTopology.h"
#include "EdgeIndex.h"
#include <math/SimpleBuffer.h>

namespace alo {

class HistoryMesh;
class PolygonUVTriangulation;

class EdgeCollapse : public MeshTopology {

    struct EdgeIndexCostPair {
        EdgeIndex _ei;
        int _ind;
        int _sortInd;
        float _cost;
/// for quick sort
        const float &key() const {
            return _cost;
        }
    };

    SimpleBuffer<EdgeIndexCostPair> m_edgeCosts;
    SimpleBuffer<EdgeIndexCostPair> m_sortedEdgeCosts;
    int m_lastSelectedEdge;
    HistoryMesh *m_mesh;
    PolygonUVTriangulation *m_triangulate;
    
public:
	EdgeCollapse();
    ~EdgeCollapse();

	void simplify(HistoryMesh *msh);

private:
/// nv reduced -1: failed 
    int processStage(int &numCoarseFaces, int &numFineFaces);
    void computeEdgeCost();
    void sortEdgesByCost();
/// very high cost edges connected to vertex i
    void removeConnectedEdgeCosts(int vi);
    EdgeIndex findEdgeToCollapse();
    
    void relocateVertices(int va, int vb,
                const std::vector<int> &vaFaces,
                const std::vector<int> &vbFaces);

/// ind area normal to each face
    void updateFaces(const std::deque<FaceIndex> &faces);
    
    void relocateFacesTo(const std::vector<int> &faces, int toLastFace);
    void insertFacesAt(const std::deque<FaceIndex> &faces, 
                        const std::deque<FaceValue> &uvs, 
                        int location);
/// area max (1 - dotN)
    void computeVertexCost(int vi);
/// ||va - vb|| max (ca, cb)
    float computeEdgeCost(EdgeValue &e, const EdgeIndex &ei) const;
    bool canEdgeCollapse(const EdgeIndex &ei);
    bool canEndProcess() const;
    void mapConnectedFaces(int vi);
/// one ring around i-th vertex
    bool getTriangulatePolygon(int i);
    int getRedVertex(const EdgeIndex &ei) const;
    
    static void PrintCollapseEdgeError(int va, int vb,
            const VertexValue &a,
            const VertexValue &b);
    static void PrintConnectedPastWarning(int va, int vb,
            const VertexValue &a,
            const VertexValue &b); 

    static const float InvalidCost;
    static const float CheckInvalidCost;
    static const float HighCost;
    static const float CheckHighCost;
};
	
}

#endif
