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

namespace alo {

class Vector3F;
class FaceIndex;

class PolygonTriangulation {

	std::vector<int> m_vertices;
	const Vector3F *m_pos;

public:
	PolygonTriangulation();
	~PolygonTriangulation();

	void setPositions(const Vector3F *pos);

	std::vector<int> &vertices();

	bool getTriangles(std::deque<FaceIndex> &faces);

private:
	int selectCorner() const;
/// http://mathworld.wolfram.com/Circumradius.html
	float getCircumRadius(const Vector3F &p1, 
					const Vector3F &p2,
					const Vector3F &p3) const;
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
