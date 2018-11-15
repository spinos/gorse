/*
 *  PolygonUVTriangulation.cpp
 *
 *  
 */

#include "PolygonUVTriangulation.h"
#include "FaceIndex.h"
#include "FaceValue.h"

namespace alo {

PolygonUVTriangulation::PolygonUVTriangulation()
{}

PolygonUVTriangulation::~PolygonUVTriangulation()
{ clearUVLookup(); }

void PolygonUVTriangulation::clearUVLookup()
{ m_uvLookup.clear(); }

void PolygonUVTriangulation::mapFace(const FaceIndex &fi, const FaceValue &fa)
{
	m_uvLookup[fi.v0()] = fa.vertexUV(fi.v0());
	m_uvLookup[fi.v1()] = fa.vertexUV(fi.v1());
	m_uvLookup[fi.v2()] = fa.vertexUV(fi.v2());
}

void PolygonUVTriangulation::getTriangleFaces(std::deque<FaceValue> &faces,
	            const std::deque<FaceIndex> &inds)
{
	std::deque<FaceIndex>::const_iterator it = inds.begin();
	for(;it!=inds.end();++it) {
		const FaceIndex &fi = *it;
		faces.push_back(FaceValue());
		FaceValue &f = faces.back();
		f.setVertexUV(fi.v0(), m_uvLookup[fi.v0()], 
						fi.v1(), m_uvLookup[fi.v1()], 
						fi.v2(), m_uvLookup[fi.v2()]);
	}
}

}
