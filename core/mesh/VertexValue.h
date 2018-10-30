/*
 *  VertexValue.h
 *
 *  vertex-to-face connection
 *  cost of collapes <- max (1 - Na dot Nb) 
 *  Na is vertex normal
 *  Nb is connected vertex normal
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
	float m_cost;
	bool m_locked;

public:

	VertexValue();
	~VertexValue();

	const float &cost() const;
	float &cost();
	void connectToFace(const FaceIndex &x);
	void disconnectFace(const FaceIndex &x);
	void clearFaces();
	void lock();
	void unlock();
	//float computeCost(const int &vi, const Vector3F *normals);
	void getConnectedVertices(std::deque<int> &vs,
			int vi) const;
/// faces created by collapesing from va to vb
	void getCollapsedFaces(std::deque<FaceIndex> &faces, int va, int vb) const;
/// reform vi to vb
	void getReformedFaces(std::deque<FaceIndex> &faces, int vi, int vb) const;
	const bool &isLocked() const;
	bool check(int vi) const;

	const std::deque<FaceIndex> &connectedFaces() const;
	int lastConnectedVertex() const;
	bool getOneRing(std::vector<int> &vertInds, int vi,
		const Vector3F *pos, const Vector3F &nv) const;

	void higherCost(const Vector3F &Na,
					const Vector3F &Nb);

	friend std::ostream& operator<<(std::ostream &output, const VertexValue & p);

private:
	void addToVector(std::deque<int> &vs, int x) const;
/// opposite to (b, vi) but not a
	bool findNextVertex(int &c, const int &a, const int &b, const int &vi) const;

};

}

#endif
