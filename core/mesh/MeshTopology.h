/*
 *  MeshTopology.h
 *  aloe
 *
 *  M = (V,E,F)
 *  past F has no edge connected
 *
 */

#ifndef ALO_MESH_TOPOLOGY_H
#define ALO_MESH_TOPOLOGY_H

#include <sdb/L3Tree.h>
#include "FaceValue.h"
#include "EdgeIndex.h"
#include "EdgeValue.h"
#include "FaceIndex.h"
#include "VertexFaceConnection.h"

namespace alo {

class VertexValue;
struct Float2;

namespace ver1 {
class ATriangleMesh;
}

class MeshTopology {

/// origin nv nf nv-on-border
    int m_numVertices;
    int m_numFaces;
    int m_numBorderVertices;
    std::vector<VertexValue> m_vertices;
    sdb::L3Tree<EdgeIndex, EdgeValue, 2048, 512, 1024> m_edges;
    sdb::L3Tree<FaceIndex, FaceValue, 2048, 512, 1024> m_tris;
    sdb::L3Tree<FaceIndex, int, 2048, 512, 1024> m_pastFaceInds;
    VertexFaceConnection m_vertexFaceConnection;
    VertexFaceConnection m_vertexPastFaceConnection;

public:
	MeshTopology();
	~MeshTopology();

	void buildTopology(const ver1::ATriangleMesh *mesh);

	const int &numVertices() const;
	const int &numFaces() const;
	const int &numBorderVertices() const;

	const VertexValue &vertex(int i) const;
	VertexValue &vertex(int i);
	EdgeValue *edge(const EdgeIndex &ei);
	FaceValue *face(const FaceIndex &fi);

    int numEdges() const;
    int numFacesConnectedTo(int vi) const;
    void printDetail() const;

protected:
/// face and edge connected to vertex[vi]
    bool removeVertexEdgeFace(int vi);
/// add face edge vertex connection
	bool addFaces(const std::deque<FaceIndex> &faces,
                const std::deque<FaceValue> &uvs,
                int lastV = 1<<30);
/// replace va by vb with uv
    void reformFaces(std::deque<FaceIndex> &inds,
                    std::deque<FaceValue> &uvs,
                    int va, int vb);

	bool checkTopology(const ver1::ATriangleMesh *mesh);
	bool setFaceInd(const FaceIndex &fi, int x);
	bool setPastFaceInd(const FaceIndex &fi, int x);
	void setMeshFaceInd(const ver1::ATriangleMesh *mesh, int i);
	
	bool faceExists(const FaceIndex &fi);
	bool pastFaceExists(const FaceIndex &fi);
/// connected to edge on border
    bool isVertexOnBorder(int vi, const VertexValue &vert,
                const ver1::ATriangleMesh *mesh);
    bool isVertexOnUVBorder(int vi, const VertexValue &vert,
                const Float2 *uvs);
    void lockFaces(const std::deque<FaceIndex> &faces);
    void lockVertices(const std::vector<int> &v);
    bool lockFace(const FaceIndex &fi);
    void unlockVertices(int high);

    void indexPastFaces(const ver1::ATriangleMesh *mesh, int begin, int end);
/// every past face ind +x
    void pushPastFaceIndex(int x);
    void replaceMeshVertex(ver1::ATriangleMesh *mesh, 
					const FaceIndex &fi, int va, int vb);
    void getFaceInds(std::vector<int> &faceInds,
                const std::deque<FaceValue> &faces) const;
    void indexEdges();

    typedef sdb::L3Node<EdgeIndex, EdgeValue, 1024> EdgeDataType;
	EdgeDataType *firstEdge();
	EdgeDataType *nextEdge(const EdgeDataType *x);
    const std::deque<FaceIndex> &facesConnectedTo(int vi) const;

    void copyPastFacesTo(std::vector<FaceIndex> &faces, int v) const;
    void clearPastFaces(int v);
    bool hasPastConnected(int a, int b) const;

    void copyFacesTo(std::deque<FaceIndex> &faces, int v) const;
    void clearFaces(int v);
    bool hasFaceConnected(int a, int b) const;
    
    bool faceConnectedIsOor(int vi, int lastFace);
    void getConnectedVertices(std::deque<int> &neivs, int vi) const;
    bool getVertexOneRing(std::vector<int> &vring, int vi,
                    const Vector3F *pos,
                    const Vector3F &nml) const;
/// if any inner connection of ring exists
    bool checkConcaveRing(int vi, 
                    const Vector3F *pos,
                    const Vector3F &nml);
    bool checkInnerEdges(const std::vector<int> &vring, int jump);
/// assuming no vertex within small ring
    void expandVertexRing(std::vector<int> &bigRing,
                    const std::vector<int> &smallRing) const;
    void updateVertexConcaveState(const std::vector<int> &vertexInds,
                    const Vector3F *pos, const Vector3F *nml);

	static void PrintUnmanifoldEdgeWarning(const FaceIndex &fi, 
                const EdgeValue &e,
                bool stat);

    static void PrintAddFaceWarning(const std::deque<FaceIndex> &faces, 
                bool stat);

private:
    EdgeValue *addEdge(const EdgeIndex &e);
    FaceValue *addFace(const FaceIndex &fi, const FaceValue &f);
    void addPastFace(const FaceIndex &fi, const int &i);
    bool removeFace(const FaceIndex &fi);
    bool removeEdge(const EdgeIndex &ei);
    bool removePastFace(const FaceIndex &fi);

};

}

#endif
