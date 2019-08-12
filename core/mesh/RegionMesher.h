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
	int m_voffset;

public:	
	
	RegionMesher();
	virtual ~RegionMesher();

	void clear();
	void borderBegin();
	void borderEnd();
	void finish();

	void operator<<(const Vector3F &q);

	void attach(AdaptableMesh *mesh);
	void detach();

	void triangulate(const int &border0, const bool &reverse0,
					const int &border1, const bool &reverse1);

	template<typename T>
	void triangulate(const T &rule);

protected:

private:
	void addBorderVertices(std::vector<int> &vs, const int &bi, const bool &reversed);
	
};

template<typename T>
void RegionMesher::triangulate(const T &rule)
{
	clear();
	const int nb = rule.numBorders();
	for(int i=0;i<nb;++i) {
		borderBegin();
		const Int2 &be = rule.border(i);
		for(int j=be.x;j<be.y;++j) {
			*this << rule.position(j);
		}
		borderEnd();
	}
	finish();

	const int nr = rule.numRegions();
	int b0, b1;
	bool r0, r1;
	for(int i=0;i<nr;++i) {
		rule.getRegion(b0, r0, b1, r1, i);
		triangulate(b0, r0, b1, r1);
	}
}

}

#endif
