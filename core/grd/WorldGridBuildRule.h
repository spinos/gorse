/*
 *  WorldGridBuildRule.h
 *  gorse
 *
 *
 *  2019/5/1
 */

#ifndef ALO_GRD_WORLD_GRID_BUILD_RULE_H
#define ALO_GRD_WORLD_GRID_BUILD_RULE_H

namespace alo {

namespace grd {

template<typename T, typename Tc, typename Tf>
class WorldGridBuildRule {

	Tf m_sfc;
	float m_centerCellSize[4];

public:

	WorldGridBuildRule();
	virtual ~WorldGridBuildRule();

/// x[3] is 2^n
/// object_size is much smaller than cell_size 
	void setCenterCellSize(const int *x);
/// cells touched by box 
/// b is float[6] (low, high)
/// ks is T[8] 
/// return #key 1-8
	int calcCellCoords(T *ks, const float *b) const;

	void calcCellAabb(float *b, const T &k) const;

protected:

private:

};

template<typename T, typename Tc, typename Tf>
WorldGridBuildRule<T, Tc, Tf>::WorldGridBuildRule()
{}

template<typename T, typename Tc, typename Tf>
WorldGridBuildRule<T, Tc, Tf>::~WorldGridBuildRule()
{}

template<typename T, typename Tc, typename Tf>
void WorldGridBuildRule<T, Tc, Tf>::setCenterCellSize(const int *x)
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
}

template<typename T, typename Tc, typename Tf>
int WorldGridBuildRule<T, Tc, Tf>::calcCellCoords(T *ks, const float *b) const
{
	T c[8];

	c[0] = m_sfc.computeKey(b);
	c[7] = m_sfc.computeKey(&b[3]);
	if(c[0] == c[7]) {
		ks[0] = c[0];
		return 1;
	}

	float x[3];
	x[0] = b[3];
	x[1] = b[1];
	x[2] = b[2];
	c[1] = m_sfc.computeKey(x);

	x[0] = b[0];
	x[1] = b[4];
	x[2] = b[2];
	c[2] = m_sfc.computeKey(x);

	x[0] = b[3];
	x[1] = b[4];
	x[2] = b[2];
	c[3] = m_sfc.computeKey(x);


	x[0] = b[0];
	x[1] = b[1];
	x[2] = b[5];
	c[4] = m_sfc.computeKey(x);

	x[0] = b[3];
	x[1] = b[1];
	x[2] = b[5];
	c[5] = m_sfc.computeKey(x);

	x[0] = b[0];
	x[1] = b[4];
	x[2] = b[5];
	c[6] = m_sfc.computeKey(x);

	int nk = 0;
	for(int k=0;k<2;++k) {
		if(k>0 && c[4] == c[0]) continue;

		for(int j=0;j<2;++j) {
			if(j>0 && c[k * 4 + 2] == c[k * 4]) continue;

			for(int i=0;i<2;++i) {
				if(i>0 && c[k * 4 + j * 2 + 1] == c[k * 4 + j * 2]) continue;
				
				ks[nk] = c[k * 4 + j * 2 + i];
				nk++;
			}
		}
	}
	return nk;
}

template<typename T, typename Tc, typename Tf>
void WorldGridBuildRule<T, Tc, Tf>::calcCellAabb(float *b, const T &k) const
{
	m_sfc.decodeKey(b, k);
	b[3] = b[0] + m_centerCellSize[3];
	b[4] = b[1] + m_centerCellSize[3];
	b[5] = b[2] + m_centerCellSize[3];
}

} /// end of namespace grd

} /// end of namespace alo

#endif
