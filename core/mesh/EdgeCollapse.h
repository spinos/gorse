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
#include <vector>

namespace alo {

class AdaptableMesh;
class EdgeIndex;
class VertexValue;
class EdgeValue;
class Vector3F;

class EdgeCollapse {

    AdaptableMesh *m_mesh;

public:

	EdgeCollapse();

	void simplify(AdaptableMesh *msh);

private:
    void buildTopology(std::vector<VertexValue> &vertices,
                    std::map<EdgeIndex, EdgeValue> &edges);
    void computeVertexCost(std::vector<VertexValue> &vertices,
                    std::map<EdgeIndex, EdgeValue> &edges);
    void computeEdgeCost(std::map<EdgeIndex, EdgeValue> &edges,
                    const std::vector<VertexValue> &vertices);
    EdgeIndex findEdgeToCollapse(std::vector<VertexValue> &vertices,
                    std::map<EdgeIndex, EdgeValue> &edges);
/// remove a keep b
    void getVertexToRemove(int &a, int &b,
                const EdgeIndex &ei, const std::vector<VertexValue> &vertices);
/// ||va - vb|| max (ca, cb)
	float computeEdgeCost(const EdgeIndex &ei, 
            const std::vector<VertexValue> &vert) const;
/// connected to edge on border
    bool isVertexOnBorder(int vi, const VertexValue &vert,
            std::map<EdgeIndex, EdgeValue> &edges);
/// faces vertices created by collapesing from va to vb
    void getCollapsedFaces(std::vector<int> &cfv,
            const std::vector<int> &faces,
            int va, int vb);
/// remove all edges connected to vi
    void removeConnectedEdges(std::map<EdgeIndex, EdgeValue> &edges, 
        const VertexValue &vert, int vi);
/// remove connected face from vertices connected to vi
    void disconnectedFaces(std::vector<VertexValue> &vertices,
        const VertexValue &vert, int vi);
/// add edges by face vertices length of 3 #face
    void addFaceEdges(std::map<EdgeIndex, EdgeValue> &edges,
        const std::vector<int> &faceVertices);
    void connectFaceToVertices(std::vector<VertexValue> &vertices,
        const std::vector<int> &faceVertices,
        int firstFaceInd);
    void updateCost(std::vector<VertexValue> &vertices,
        const std::vector<int> &faceVertices);
    void relocateFacesTo(const std::vector<int> &faces, int toLastFace,
        std::vector<VertexValue> &vertices);
};
	
}

#endif
