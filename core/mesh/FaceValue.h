/*
 *  FaceValue.h
 *
 *  ind area normal vertex-uv-map for each triangle
 *
 */

#ifndef ALO_FACE_VALUE_H
#define ALO_FACE_VALUE_H

#include <iostream>
#include <math/Vector3F.h>

namespace alo {

class FaceIndex;

class FaceValue
{
	float m_area;
	int m_ind;
	int m_vertexId[3];
	int m_uvId[3];
	Vector3F m_nml;
	
public:
	FaceValue();
	FaceValue(int x);
	
	void setNormal(const Vector3F &nml);
	void setArea(const float &x);
	void setVertexUV(int vertex0, int uv0,
					int vertex1, int uv1,
					int vertex2, int uv2);
	void setInd(int x);
	int &ind();
	const int &ind() const;
	const float &area() const;
	const Vector3F &normal() const;
	int vertexUV(int vi) const;
	bool replaceVertex(int a, int b);

	friend std::ostream& operator<<(std::ostream &output, const FaceValue & p)
    {
        output << " " << p.ind();
        return output;
    }
	
};

}

#endif
