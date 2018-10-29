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

class AdaptableMesh;
class VertexValue;
class EdgeIndex;
class EdgeValue;
class FaceIndex;
class FaceValue;
class Vector3F;

class EdgeCollapse {

    AdaptableMesh *m_mesh;
/// no vertex will be added or removed, they just shuffle
    VertexValue *m_vertices;
    int m_numVertices;
    int m_numFaces;
    std::map<EdgeIndex, EdgeValue> m_edges;
    std::map<FaceIndex, FaceValue> m_tris;
    
public:

	EdgeCollapse();
    ~EdgeCollapse();

	void simplify(AdaptableMesh *msh);

private:
    void buildTopology();
    void computeVertexCost();
    void computeEdgeCost();
    EdgeIndex findEdgeToCollapse();
/// remove a keep b
    void getVertexToRemove(int &a, int &b, const EdgeIndex &ei);
/// ||va - vb|| max (ca, cb)
	float computeEdgeCost(const EdgeIndex &ei) const;
/// connected to edge on border
    bool isVertexOnBorder(int vi, const VertexValue &vert);
    bool removeFaces(const VertexValue &vert, int vi);
    void removeEdge(const EdgeIndex &ei);
    bool removeFace(const FaceIndex &fi);
    void addFaces(const std::deque<FaceIndex> &faces);
    void addEdge(const EdgeIndex &e);
    void getConnectedFaceInds(std::vector<int> &faceInds,
                const VertexValue &vert);
    void relocateVertices(int va, int vb,
                const std::vector<int> &vaFaces,
                const std::vector<int> &vbFaces);
    void setFaceInd(int i);
    bool checkTopology();
    void updateCost(const std::deque<FaceIndex> &faces);
    void relocateFacesTo(const std::vector<int> &faces, int toLastFace);
    void insertFacesAt(const std::deque<FaceIndex> &faces, 
                        int location);
};
	
}

#endif
