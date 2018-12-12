/*
 *  VertexFaceConnection.h
 *  aloe
 *
 *  vertex-face look up
 *
 */

#ifndef ALO_VERTEX_FACE_CONNECTION_H
#define ALO_VERTEX_FACE_CONNECTION_H

#include "FaceIndex.h"
#include <vector>
#include <deque>

namespace alo {

class Vector3F;

class VertexFaceConnection {

	std::vector<std::deque<FaceIndex> > m_connections;

public:
	VertexFaceConnection();
	virtual ~VertexFaceConnection();

	void resize(int n);
/// v0 -> f(v0, v1, v2) v1 -> f(v0, v1, v2) v2 -> f(v0, v1, v2)
	void connectFace(const FaceIndex &fi);
	void disconnectFace(const FaceIndex &fi);
	void disconnectFace(int v, const FaceIndex &fi);
	void clearFaces(int v);

	int numFacesConnected(int v) const;
	void copyFacesTo(std::vector<FaceIndex> &faces, int v) const;
	bool hasIntersection(int a, int b) const;
/// all connected faces has v
	bool check(int v) const;
	void getConnectedVertices(std::deque<int> &vs, int vi) const;
	bool getVertexOneRing(std::vector<int> &vertInds, int vi,
				const Vector3F *pos, const Vector3F &nv) const;
	const std::deque<FaceIndex> &operator[](int v) const;
	int totalSize() const;
	void addVertexRingTo(std::vector<int> &vertInds, 
				const std::vector<int> &smallRing, int vi) const;

private:
	void connectFace(std::deque<FaceIndex> &c, const FaceIndex &x);
	void disconnectFace(std::deque<FaceIndex> &c, const FaceIndex &x);
	bool hasIntersection(const std::deque<FaceIndex> &a, const FaceIndex &b) const;
	void addToVector(std::deque<int> &vs, int x) const;
/// opposite to (b, vi) but not a
	bool findNextVertex(int &c, const int &a, const int &b, const int &vi) const;
	void expandExclude(std::vector<int> &a, const std::vector<int> &b, int vi) const;

};
	
}

#endif
