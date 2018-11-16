/*
 *  MeshTopology.h
 *
 *  M = (V,E,F)
 *  past F has no edge connected
 *
 */

#ifndef ALO_MESH_TOPOLOGY_H
#define ALO_MESH_TOPOLOGY_H

#include <map>
#include <deque>
#include <vector>

namespace alo {

class VertexValue;
class EdgeIndex;
class EdgeValue;
class FaceIndex;
class FaceValue;
struct Float2;

namespace ver1 {
class ATriangleMesh;
}

class MeshTopology {

/// origin nv nf nv-on-border
    int m_numVertices;
    int m_numFaces;
    int m_numBorderVertices;
    VertexValue *m_vertices;
    std::map<EdgeIndex, EdgeValue> m_edges;
    std::map<FaceIndex, FaceValue> m_tris;
    std::map<FaceIndex, FaceValue> m_pastFaces;

public:
	MeshTopology();
	~MeshTopology();

	void buildTopology(const ver1::ATriangleMesh *mesh);

	const int &numVertices() const;
	const int &numFaces() const;
	const int &numBorderVertices() const;

	const VertexValue &vertex(int i) const;
	VertexValue &vertex(int i);
	EdgeValue &edge(const EdgeIndex &ei);
	FaceValue &face(const FaceIndex &fi);

protected:
	void addEdge(const EdgeIndex &e);
	void addFace(const FaceIndex &fi, const FaceValue &f);
	bool addFaces(const std::deque<FaceIndex> &faces,
                const std::deque<FaceValue> &uvs,
                int lastV = 1<<30);
/// replace va by vb
    void reformFaces(std::deque<FaceIndex> &inds,
                    std::deque<FaceValue> &uvs,
                    int va, int vb);
	bool checkTopology(const ver1::ATriangleMesh *mesh);
	bool setFaceInd(const FaceIndex &fi, int x);
	bool setPastFaceInd(const FaceIndex &fi, int x);
	void setMeshFaceInd(const ver1::ATriangleMesh *mesh, int i);
	bool removeFace(const FaceIndex &fi);
	bool removeEdge(const EdgeIndex &ei);
/// face and edge connected to vertex[vi]
    bool removeVertexConnection(int vi);
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
    void connectFaceToVertex(const FaceIndex &fi);
    void connectFaceToVertexPast(const FaceIndex &fi);
    void connectFaceToEdge(const EdgeIndex &ei, const FaceIndex &fi, bool &stat);
    void addPastFace(const FaceIndex &fi, const FaceValue &f);
    bool removePastFace(const FaceIndex &fi);
    void indexPastFaces(const ver1::ATriangleMesh *mesh, int begin, int end);
/// every past face ind +x
    void pushPastFaceIndex(int x);
    void replaceMeshVertex(ver1::ATriangleMesh *mesh, 
					const FaceIndex &fi, int va, int vb);
    void getFaceInds(std::vector<int> &faceInds,
                const std::deque<FaceValue> &faces) const;
	std::map<EdgeIndex, EdgeValue>::iterator edgesBegin();
	std::map<EdgeIndex, EdgeValue>::iterator edgesEnd();

	static void PrintUnmanifoldEdgeWarning(const FaceIndex &fi, 
                const EdgeValue &e,
                bool stat);

    static void PrintAddFaceWarning(const std::deque<FaceIndex> &faces, 
                bool stat);

private:

};

}

#endif
