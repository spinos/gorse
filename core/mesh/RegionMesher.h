/*
 *  RegionMesher.h
 *  gorse
 *
 *  position sequence separated by borders (begin, end)
 *  triangulate regions enclosed by 2 borders
 *  2019/8/12
 */

#ifndef ALO_MESH_REGION_MESHER_H
#define ALO_MESH_REGION_MESHER_H

#include <math/SimpleBuffer.h>
#include <math/Vector3F.h>
#include <math/Int2.h>
#include <vector>
#include <deque>

namespace alo {

class AdaptableMesh;
	
class RegionMesher {

	AdaptableMesh *m_mesh;
	SimpleBuffer<Vector3F> m_positions;
	std::deque<Int2> m_borders;

public:	
	
	RegionMesher();
	virtual ~RegionMesher();

	void clear();
	void borderBegin();
	void borderEnd();
	void finish();

	void operator<<(const Vector3F &q);

	void attach(AdaptableMesh *mesh);
	void triangulate(const int &border0, const bool &reverse0,
					const int &border1, const bool &reverse1);
	void detach();

protected:

private:
	void addBorderVertices(std::vector<int> &vs, const int &bi, const bool &reversed);
	Vector3F getRegionCenter(const std::vector<int> &vs) const;

};

}

#endif
