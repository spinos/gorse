/*
 *  PolygonUVTriangulation.h
 *
 *  produce face-varying uv for resulting triangles
 *  assuming vertex uv on different constituent faces are close enough
 *
 */

#ifndef ALO_POLYGON_UV_TRIANGULATION_H
#define ALO_POLYGON_UV_TRIANGULATION_H

#include "PolygonTriangulation.h"
#include <map>

namespace alo {

class FaceValue;

class PolygonUVTriangulation : public PolygonTriangulation {

/// uv <- map(vertex)
	std::map<int, int> m_uvLookup;

public:
	PolygonUVTriangulation();
	virtual ~PolygonUVTriangulation();

	void clearUVLookup();

	void mapFace(const FaceIndex &fi, const FaceValue &fa);

	void getTriangleFaces(std::deque<FaceValue> &faces,
	            const std::deque<FaceIndex> &inds);

};

}

#endif
