#include "PolygonTriangulation.h"
#include "FaceIndex.h"

namespace alo {

PolygonTriangulation::PolygonTriangulation()
{}

PolygonTriangulation::~PolygonTriangulation()
{ m_vertices.clear(); }

void PolygonTriangulation::setPositions(const Vector3F *pos)
{ m_pos = pos; }

void PolygonTriangulation::setCenter(const Vector3F &c)
{ m_center = c; }

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

	return faces.size() == ntri;
}

int PolygonTriangulation::selectCorner() const
{
	int ncorner = m_vertices.size();
	if(ncorner < 4) return 0;
	
	float minRadius = 1e28f;
	int a, b, c, sel=0;
	for(int i=0;i<ncorner;++i) {
		
		getTriangleVertices(a,b,c,i);
		if(isCornerConcave(m_pos[a], m_pos[b], m_pos[c]))
			continue;
		
		float r = getCircumRadius(m_pos[a], m_pos[b], m_pos[c]);
		if(minRadius > r) {
			minRadius = r;
			sel = i;
		}
	}
	//if(minRadius > 1e27f) std::cout<<"\n concave corner?" << ncorner;
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

bool PolygonTriangulation::isCornerConcave(const Vector3F &a, 
					const Vector3F &b,
					const Vector3F &c) const
{
	Vector3F ab = b - a;
	Vector3F ac = c - a;
	Vector3F nf = ab.cross(ac);
	Vector3F ax = m_center - a;
	if(ab.cross(ax).dot(nf) < 0.f) return true;
	if(ax.cross(ac).dot(nf) < 0.f) return true;
	return false;
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
