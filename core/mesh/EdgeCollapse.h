/*
 *  Mesh simplification by edge collapsing
 *
 *  find edge with lowest cost to collapse
 *  relocate remove vertex and faces to end
 *
 */

#ifndef ALO_EDGE_COLLAPSE_H
#define ALO_EDGE_COLLAPSE_H

#include <map>
#include <deque>
#include <vector>

namespace alo {

class HistoryMesh;
class VertexValue;
class EdgeIndex;
class EdgeValue;
class FaceIndex;
class FaceValue;
class Vector3F;
class PolygonTriangulation;

class EdgeCollapse {

    HistoryMesh *m_mesh;
    PolygonTriangulation *m_triangulate;
/// no vertex will be added or removed, they just shuffle
    VertexValue *m_vertices;
    int m_numVertices;
    int m_numFaces;
    int m_numBorderVertices;
    std::map<EdgeIndex, EdgeValue> m_edges;
    std::map<FaceIndex, FaceValue> m_tris;
    
public:

	EdgeCollapse();
    ~EdgeCollapse();

	void simplify(HistoryMesh *msh);

private:
    void buildTopology();
    void computeEdgeCost();
    bool processStage();
    EdgeIndex findEdgeToCollapse();
/// remove a keep b
    void getVertexToRemove(int &a, int &b, const EdgeIndex &ei);
/// connected to edge on border
    bool isVertexOnBorder(int vi, const VertexValue &vert);
    bool removeFaces(const VertexValue &vert, int vi);
    void removeEdge(const EdgeIndex &ei);
    bool addFaces(const std::deque<FaceIndex> &faces,
                int lastV = 1<<30);
    void addEdge(const EdgeIndex &e);
    void getConnectedFaceInds(std::vector<int> &faceInds,
                const VertexValue &vert);
    bool lastConnectedFaceOor(const VertexValue &vert);
    void relocateVertices(int va, int vb,
                const std::vector<int> &vaFaces,
                const std::vector<int> &vbFaces);
    void setFaceInd(int i);
    bool checkTopology();
    void updateCost(const std::deque<FaceIndex> &faces);
    void lockVertices(const std::deque<FaceIndex> &faces);
    void unlockAllVertices();
    void relocateFacesTo(const std::vector<int> &faces, int toLastFace);
    void insertFacesAt(const std::deque<FaceIndex> &faces, 
                        int location);
/// area max (1 - dotN)
    void computeVertexCost(VertexValue &vert, int vi);
/// ||va - vb|| max (ca, cb)
    void computeEdgeCost(EdgeValue &e, const EdgeIndex &ei) const;
    bool canEdgeCollapse(const EdgeIndex &ei);
    bool canEndProcess() const;

    static void PrintUnmanifoldEdgeWarning(const FaceIndex &fi, 
                const EdgeValue &e,
                bool stat);

    static void PrintAddFaceWarning(const std::deque<FaceIndex> &faces, 
                bool stat);

    static void PrintCollapseEdgeError(int va, int vb,
        const VertexValue &a,
            const VertexValue &b);

};
	
}

#endif
