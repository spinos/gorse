/*
 *  VertexValue.h
 *
 *  vertex-to-face connection
 *
 */

#ifndef ALO_VERTEX_VALUE_H
#define ALO_VERTEX_VALUE_H

#include <vector>
#include <deque>
#include "FaceIndex.h"

namespace alo {

class Vector3F;

class VertexValue
{
	std::deque<FaceIndex> m_faceInds;
	std::deque<FaceIndex> m_pastFaceInds;
	float m_cost;
	bool m_locked;
	bool m_onborder;

public:

	VertexValue();
	~VertexValue();

	const float &cost() const;
	float &cost();
	void connectToFace(const FaceIndex &x);
	void disconnectFace(const FaceIndex &x);
	void connectToPastFace(const FaceIndex &x);
	void disconnectPastFace(const FaceIndex &x);
	void clearFaces();
	void clearPastFaces();
	void lock();
	void unlock();
	void setOnBorder(bool x);
	const bool &isOnBorder() const;
	void getConnectedVertices(std::deque<int> &vs,
			int vi) const;
/// reform vi to vb
	void getReformedFaces(std::deque<FaceIndex> &faces, int vi, int vb) const;
	const bool &isLocked() const;
/// all faces has vi
	bool checkFaces(int vi) const;

	bool getOneRing(std::vector<int> &vertInds, int vi,
		const Vector3F *pos, const Vector3F &nv) const;

	void copyPastFacesTo(std::vector<FaceIndex> &faces) const;
	
	bool hasFaceConnected(const VertexValue &b) const;
	bool hasFaceConnected(const FaceIndex &fi) const;

	bool hasPastConnected(const VertexValue &b) const;
	bool hasPastConnected(const FaceIndex &fi) const;

	std::deque<FaceIndex>::const_iterator facesBegin() const;
	std::deque<FaceIndex>::const_iterator facesEnd() const;
	std::deque<FaceIndex>::const_iterator pastFacesBegin() const;
	std::deque<FaceIndex>::const_iterator pastFacesEnd() const;

	int numConnectedFaces() const;
	int numConnectedPastFaces() const;

	friend std::ostream& operator<<(std::ostream &output, const VertexValue & p);

private:
	void addToVector(std::deque<int> &vs, int x) const;
/// opposite to (b, vi) but not a
	bool findNextVertex(int &c, const int &a, const int &b, const int &vi) const;

};

}

#endif
