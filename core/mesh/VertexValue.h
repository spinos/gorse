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

namespace alo {

class Vector3F;

class VertexValue
{
	std::vector<int> m_faceIndices;
	float m_cost;

public:

	VertexValue();
	~VertexValue();

	void connectToFace(int i);
	void removeFaceConnectedTo(const int &vi,
					const unsigned *indices);
	void swapFace(int a, int b);

	float computeCost(const int &vi,
					const unsigned *indices,
					const Vector3F *normals);

	const float &cost() const;
	const std::vector<int> &faceIndices() const;
	int numFaces() const;

	void getConnectedVertices(std::vector<int> &vs,
			int vi, const unsigned *indices) const;

	void operator=(const VertexValue &b);

private:
	void higherCost(const Vector3F &Na,
					const Vector3F &Nb);
	void addToVector(std::vector<int> &vs, int x) const;

};

}

#endif
