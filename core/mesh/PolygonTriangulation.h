/*
 *  PolygonTriangulation.h
 *
 *  triangulate a polygon by a list of vertices
 *  ngon has n-2 triangles
 *  select corner with minimal circumcircle radius
 *
 */

#ifndef ALO_POLYGON_TRIANGULATION_H
#define ALO_POLYGON_TRIANGULATION_H

#include <vector>
#include <deque>
#include <math/Vector3F.h>

namespace alo {

class FaceIndex;

class PolygonTriangulation {

	std::vector<int> m_vertices;
	Vector3F m_center;
	const Vector3F *m_pos;

public:
	PolygonTriangulation();
	~PolygonTriangulation();

	void setPositions(const Vector3F *pos);
	void setCenter(const Vector3F &c);

	std::vector<int> &vertices();

	bool getTriangles(std::deque<FaceIndex> &faces);

private:
	int selectCorner() const;
/// http://mathworld.wolfram.com/Circumradius.html
	float getCircumRadius(const Vector3F &p1, 
					const Vector3F &p2,
					const Vector3F &p3) const;
///             b
///            /
/// center - a
///            \
///             c
	bool isCornerConcave(const Vector3F &a, 
					const Vector3F &b,
					const Vector3F &c) const;
	void addTriangle(std::deque<FaceIndex> &faces, int i) const;
/// b    i+1
/// | \
/// |  a i
/// | /
/// c    i-1
	void getTriangleVertices(int &a, int &b, int &c, int i) const;

};

}

#endif
