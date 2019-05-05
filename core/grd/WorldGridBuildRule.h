/*
 *  WorldGridBuildRule.h
 *  gorse
 *
 *
 *  2019/5/1
 */

#ifndef ALO_GRD_WORLD_GRID_BUILD_RULE_H
#define ALO_GRD_WORLD_GRID_BUILD_RULE_H

#include <math/SimpleBuffer.h>

namespace alo {

namespace grd {

template<typename T, typename Tf>
class WorldGridBuildRule {

	BoundingBox m_bbox;
	Tf m_sfc;
	float m_centerCellSize[4];
	SimpleBuffer<T> m_ks;

public:

	WorldGridBuildRule();
	virtual ~WorldGridBuildRule();

/// x[3] is 2^n
/// object_size is much smaller than cell_size 
	void setCenterCellSize(const int *x);
/// cells touched by box 
/// b is float[6] (low, high)
/// cell keys stored in m_ks
	void calcCellCoords(const float *b);

	void calcCellAabb(float *b, const T &k) const;
	
	T computeKey(const float *x) const;

	const BoundingBox &bbox() const;

	int numKeys() const;
	const T &key(int i) const;

protected:

private:

};

template<typename T, typename Tf>
WorldGridBuildRule<T, Tf>::WorldGridBuildRule()
{}

template<typename T, typename Tf>
WorldGridBuildRule<T, Tf>::~WorldGridBuildRule()
{}

template<typename T, typename Tf>
void WorldGridBuildRule<T, Tf>::setCenterCellSize(const int *x)
{
	m_centerCellSize[0] = x[0];
	m_centerCellSize[1] = x[1];
	m_centerCellSize[2] = x[2];
	m_centerCellSize[3] = x[3];

	float centerHalfSpan[4];
	int h = x[3]>>1;
	centerHalfSpan[0] = x[0] - h;
	centerHalfSpan[1] = x[1] - h;
	centerHalfSpan[2] = x[2] - h;
	centerHalfSpan[3] = x[3]<<9;
	m_sfc.setCoord(centerHalfSpan);

	m_bbox.setMin(centerHalfSpan[0] - centerHalfSpan[3], 
		centerHalfSpan[1] - centerHalfSpan[3],
		centerHalfSpan[2] - centerHalfSpan[3]);
	m_bbox.setMax(centerHalfSpan[0] + centerHalfSpan[3], 
		centerHalfSpan[1] + centerHalfSpan[3],
		centerHalfSpan[2] + centerHalfSpan[3]);

	std::cout << "\n world box " << m_bbox;
}

template<typename T, typename Tf>
void WorldGridBuildRule<T, Tf>::calcCellCoords(const float *b)
{
	m_ks.purgeBuffer();

	const float &h = m_centerCellSize[3];
	const float *bx = m_bbox.data();

	int u0[3];
	u0[0] = (b[0] - bx[0]) / h; 
	u0[1] = (b[1] - bx[1]) / h; 
	u0[2] = (b[2] - bx[2]) / h; 

	int u1[3];
	u1[0] = (b[3] - bx[0]) / h; 
	u1[1] = (b[4] - bx[1]) / h; 
	u1[2] = (b[5] - bx[2]) / h; 

	float p[3];

	for(int k=u0[2];k<=u1[2];++k) {

		p[2] = bx[2] + h * k;

		for(int j=u0[1];j<=u1[1];++j) {

			p[1] = bx[1] + h * j;

			for(int i=u0[0];i<=u1[0];++i) {

				p[0] = bx[0] + h * i;

				T c = m_sfc.computeKey(p);

				m_ks << c;
			}
		}
	}

}

template<typename T, typename Tf>
T WorldGridBuildRule<T, Tf>::computeKey(const float *x) const
{ return m_sfc.computeKey(x); }

template<typename T, typename Tf>
void WorldGridBuildRule<T, Tf>::calcCellAabb(float *b, const T &k) const
{
	m_sfc.decodeKey(b, k);
	b[3] = b[0] + m_centerCellSize[3];
	b[4] = b[1] + m_centerCellSize[3];
	b[5] = b[2] + m_centerCellSize[3];
}

template<typename T, typename Tf>
const BoundingBox &WorldGridBuildRule<T, Tf>::bbox() const
{ return m_bbox; }

template<typename T, typename Tf>
int WorldGridBuildRule<T, Tf>::numKeys() const
{ return m_ks.count(); }

template<typename T, typename Tf>
const T &WorldGridBuildRule<T, Tf>::key(int i) const
{ return m_ks[i]; }

} /// end of namespace grd

} /// end of namespace alo

#endif
