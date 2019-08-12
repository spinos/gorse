/*
 *  RegionMesher.cpp
 *  gorse
 *
 *  triangulate regions
 *  2019/8/12
 */

#include "RegionMesher.h"
#include "PolygonTriangulation.h"
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
{}

void RegionMesher::operator<<(const Vector3F &q)
{
	m_positions<<q;
	Int2 &b = m_borders.back();
	b.y++;
}

void RegionMesher::attach(AdaptableMesh *mesh)
{ 
	m_mesh = mesh;
	const int &n = m_positions.count();
	for(int i=0;i<n;++i)
		mesh->addVertex(m_positions[i]);
}

void RegionMesher::triangulate(const int &border0, const bool &reverse0,
				const int &border1, const bool &reverse1)
{
	PolygonTriangulation poly;
	poly.setPositions(m_positions.c_data());
	std::vector<int> &polyVs = poly.vertices();
	addBorderVertices(polyVs, border0, reverse0);
	addBorderVertices(polyVs, border1, reverse1);
	Vector3F cen = getRegionCenter(polyVs);
	poly.setCenter(cen);
}

void RegionMesher::detach()
{ m_mesh->calculateVertexNormals(); }

void RegionMesher::addBorderVertices(std::vector<int> &vs, const int &bi, const bool &reversed)
{
	const Int2 &b = m_borders[bi];
	if(reversed) {
		for(int i=b.y-1;i>=b.x;++i) {
			vs.push_back(i);
		}
	} else {
		for(int i=b.x;i<b.y;++i) {
			vs.push_back(i);
		}
	}
}

Vector3F RegionMesher::getRegionCenter(const std::vector<int> &vs) const
{
	Vector3F cen(0.f, 0.f, 0.f);
	std::vector<int>::const_iterator it = vs.begin();
	for(;it!=vs.end();++it) {
		cen += m_positions[*it];
	}
	cen *= 1.f / (float)vs.size();
	return cen;
}

}
