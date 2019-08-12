/*
 *  RegionMesher.cpp
 *  gorse
 *
 *  triangulate regions
 *  2019/8/12
 */

#include "RegionMesher.h"
#include "PolygonTriangulation.h"
#include "FaceIndex.h"
#include <geom/AdaptableMesh.h>

namespace alo {

RegionMesher::RegionMesher()
{}

RegionMesher::~RegionMesher()
{}

void RegionMesher::clear()
{
	m_positions.purgeBuffer();
	m_borders.clear();
}

void RegionMesher::borderBegin()
{
	int nv = m_positions.count();
	Int2 b(nv, nv);
	m_borders.push_back(b);
}

void RegionMesher::borderEnd()
{}

void RegionMesher::finish()
{
	m_voffset = m_mesh->numVertices();
	const int &n = m_positions.count();
	for(int i=0;i<n;++i)
		m_mesh->addVertex(m_positions[i]);

	std::cout << "\n finish "<<m_voffset<<" + "<<n;
}

void RegionMesher::operator<<(const Vector3F &q)
{
	m_positions<<q;
	Int2 &b = m_borders.back();
	b.y++;
}

void RegionMesher::attach(AdaptableMesh *mesh)
{ 
	m_mesh = mesh;
}

void RegionMesher::triangulate(const int &border0, const bool &reverse0,
				const int &border1, const bool &reverse1)
{
	PolygonTriangulation poly;
	poly.setPositions(m_positions.c_data());
	std::vector<int> &polyVs = poly.vertices();
	addBorderVertices(polyVs, border0, reverse0);
	addBorderVertices(polyVs, border1, reverse1);
	poly.calculateCenterAndNormal();

	std::cout << "\n region center " << poly.center() << " normal " << poly.normal();

	std::deque<FaceIndex> faces;
	poly.getTriangles(faces);

	std::deque<FaceIndex>::const_iterator it = faces.begin();
	for(;it!=faces.end();++it) {
		const FaceIndex &fi = *it;
		if(poly.isFaceNormalReversed(fi.v0(), fi.v1(), fi.v2()))
			m_mesh->addTriangle(m_voffset + fi.v0(), m_voffset + fi.v2(), m_voffset + fi.v1());
		else
			m_mesh->addTriangle(m_voffset + fi.v0(), m_voffset + fi.v1(), m_voffset + fi.v2());
	}
}

void RegionMesher::detach()
{ m_mesh->calculateVertexNormals(); }

void RegionMesher::addBorderVertices(std::vector<int> &vs, const int &bi, const bool &reversed)
{
	const Int2 &b = m_borders[bi];
	if(reversed) {
		for(int i=b.y-1;i>=b.x;--i) {
			vs.push_back(i);
		}
	} else {
		for(int i=b.x;i<b.y;++i) {
			vs.push_back(i);
		}
	}
}

}
