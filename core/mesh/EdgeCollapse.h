/*
 *  Mesh simplification by edge collapsing
 *
 *  find edge with lowest cost to collapse
 *  relocate remove vertex and faces to end
 *  no vertex will be added or removed, they just shuffle
 *  cost of collapes a vertex <- max (1 - Na dot Nb)
 *  Na, Nb is normal of any face pair connected to the vertex
 *
 */

#ifndef ALO_EDGE_COLLAPSE_H
#define ALO_EDGE_COLLAPSE_H

#include "MeshTopology.h"

namespace alo {

class HistoryMesh;
class PolygonTriangulation;

class EdgeCollapse : public MeshTopology {

    HistoryMesh *m_mesh;
    PolygonTriangulation *m_triangulate;
    
public:

	EdgeCollapse();
    ~EdgeCollapse();

	void simplify(HistoryMesh *msh);

private:
    void computeEdgeCost();
/// nv reduced -1: failed 
    int processStage(int &numCoarseFaces, int &numFineFaces);
    EdgeIndex findEdgeToCollapse();
/// remove a keep b
    void getVertexToRemove(int &a, int &b, const EdgeIndex &ei);
    bool lastConnectedFaceOor(const VertexValue &vert);
    void relocateVertices(int va, int vb,
                const std::vector<int> &vaFaces,
                const std::vector<int> &vbFaces);

    void updateCost(const std::deque<FaceIndex> &faces);
    void updateCost(const std::deque<FaceIndex> &faces,
                const std::vector<int> &vertices);
/// ind area normal to each face
    void updateFaces(const std::deque<FaceIndex> &faces);
    
    void relocateFacesTo(const std::vector<int> &faces, int toLastFace);
    void insertFacesAt(const std::deque<FaceIndex> &faces, 
                        int location);
/// area max (1 - dotN)
    void computeVertexCost(VertexValue &vert);
/// ||va - vb|| max (ca, cb)
    void computeEdgeCost(EdgeValue &e, const EdgeIndex &ei) const;
    void computeEdgeCost(const std::deque<FaceIndex> &faces);
    bool canEdgeCollapse(const EdgeIndex &ei);
    bool canEndProcess() const;
    
    static void PrintCollapseEdgeError(int va, int vb,
            const VertexValue &a,
            const VertexValue &b);
    static void PrintConnectedPastWarning(int va, int vb,
            const VertexValue &a,
            const VertexValue &b); 
    static void PrintOneRing(int va, const std::vector<int> &ring);

};
	
}

#endif
