#include "PolygonTriangulation.h"
#include "FaceIndex.h"
#include <math/Vector3F.h>

namespace alo {

PolygonTriangulation::PolygonTriangulation()
{}

PolygonTriangulation::~PolygonTriangulation()
{ m_vertices.clear(); }

void PolygonTriangulation::setPositions(const Vector3F *pos)
{ m_pos = pos; }

std::vector<int> &PolygonTriangulation::vertices()
{ return m_vertices; }

bool PolygonTriangulation::getTriangles(std::deque<FaceIndex> &faces)
{
	const int ntri = m_vertices.size() - 2;
	if(ntri < 1) 
		return false;
	if(ntri == 1) {
		faces.push_back( FaceIndex(m_vertices[0], m_vertices[1], m_vertices[2]) );
		return true;
	}

	for(int i=0;i<ntri;++i) {
		const int c = selectCorner();
		addTriangle(faces, c);
		m_vertices.erase(m_vertices.begin() + c);
	}

	return true;
}

int PolygonTriangulation::selectCorner() const
{
	int ncorner = m_vertices.size();
	if(ncorner < 4) return 0;
	if(ncorner < 5) ncorner = 2;

	float minRadius = 1e28f;
	int a, b, c, sel=0;
	for(int i=0;i<ncorner;++i) {
		
		getTriangleVertices(a,b,c,i);
		float r = getCircumRadius(m_pos[a], m_pos[b], m_pos[c]);
		if(minRadius > r) {
			minRadius = r;
			sel = i;
		}
	}
	return sel;
}

void PolygonTriangulation::addTriangle(std::deque<FaceIndex> &faces, int i) const
{
	int a, b, c;
	getTriangleVertices(a,b,c,i);
	faces.push_back(FaceIndex(a, b, c));
}

void PolygonTriangulation::getTriangleVertices(int &a, int &b, int &c, int i) const
{
	a = m_vertices[i];
	const int lastV = m_vertices.size() - 1;

	int vb = i + 1;
	if(vb > lastV) vb = 0;
	b = m_vertices[vb];

	int vc = i - 1;
	if(vc < 0) vc = lastV;
	c = m_vertices[vc];
}

float PolygonTriangulation::getCircumRadius(const Vector3F &p1, 
					const Vector3F &p2,
					const Vector3F &p3) const
{
	float a = p1.distanceTo(p2);
	float b = p2.distanceTo(p3);
	float c = p3.distanceTo(p1);
	return a * b * c / sqrt((a + b + c) * (b + c - a) * (c + a - b) * (a + b - c));
}

}
