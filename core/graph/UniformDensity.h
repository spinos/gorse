/*
 *  UniformDensity.h
 *  aloe
 *  
 *  estimate normal direction by density gradient
 *  uniform grid of M-by-N-by-P dimension
 *  densities are stored at cell centers
 *  at 8 sub-cells find sign changes across 8 cells
 *  [0,7] solid cells
 *  0 1  0 1  0 1       0 1
 *  0 0  0 1  1 1 good  1 0 bad
 *  node at cell center, edges are axix-aligned and have same length
 *  
 *  Created by jian zhang on 2/10/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_SDS_UNIFORM_DENSITY_H
#define ALO_SDS_UNIFORM_DENSITY_H

#include <graph/BaseDistanceField.h>
#include <boost/scoped_array.hpp>

namespace alo {

struct Float4;
class Vector3F;
class BoundingBox;

class UniformDensity : public BaseDistanceField {

/// (mean_position, density)
	boost::scoped_array<Float4> m_rho;

	struct DensityFront {
/// (cell_i,j,k,child_id)
        int _ind[4];
		float _nml[3];
	};
/// where the boundary is
	boost::scoped_array<DensityFront> m_front;
	float m_subDensity[32];
	int m_numFronts;
/// (m,n,p,m*n*p)
	int m_dim[4];
/// (x,y,z,h,1/h)
	float m_originSize[5];
	
public:
	
	UniformDensity();
	virtual ~UniformDensity();
	
	void create(int M, int N, int P,
				const float* boxOrigin,
				const float& cellSize);

/// reset				
	void setOriginAndCellSize(const float* boxOrigin,
				const float& cellSize);
	
/// rho <- 0				
	void setZero();
/// accumulate in cells
	bool accumulate(const float& val, const Vector3F& p);
	void finish();

	bool isEmpty() const;
	const int& numCells() const;
	const int* dimension() const;
	const float* originCellSize() const;
	const float& cellSize() const;
	const float& getDensity(int i, int j, int k) const;
	float safeGetDensity(int i, int j, int k) const;
	void safeGetPositionDensity(float* dst, int i, int j, int k) const;
	Vector3F getCellCenter(int i, int j, int k) const;
	BoundingBox getCellBox(int i, int j, int k) const;
	float getNodeValue(int i, int j, int k) const;
	const int& numFronts() const;
	void getFront(BoundingBox& frontBx, Vector3F& frontNml, const int& i) const;
	
	template<typename T, typename Ta>
	void buildFrontSamples(T& asamp, Ta* dst);
	
private:
/// num_cell nodes
/// (M-1)NP + M(N-1)P + MN(P-1) edges
	void buildGraph();
/// detect front for 8 subcell int cell(i,j,k) 
    void detectFrontInCell(const int& i, const int& j, const int& k);
/// s-th subcell in cell(i,j,k)
    bool detectSubCellFront(const int& i, const int& j, const int& k, const int& s);
/// by sud cell density delta
	void estimateNormal(DensityFront* dst) const;
/// s-th subcell in cell(i,j,k)
	void getSubCellBox(BoundingBox& b, int i, int j, int k, int s) const;
    void getSubCellCenter(float* p, int i, int j, int k, int s) const;
    void getFrontCellCenter(float* p, const DensityFront& ft) const;
	int cellInd(int i, int j, int k) const;
	
	int firstEmptyCellInd() const;
	
	void aggregateFrontSamplePos(Vector3F& pos, const DensityFront& fi);
	
	static const int EightSubCellCoord[8][3];
    static const int EightSubCellNeighborXOffset[8][8];
    static const int EightSubCellNeighborYOffset[8][8];
    static const int EightSubCellNeighborZOffset[8][8];
    static const int EightSubCellOffset[8][3];
    
};

template<typename T, typename Ta>
void UniformDensity::buildFrontSamples(T& asamp, Ta* dst)
{
	for(int i=0;i<m_numFronts;++i) {
		const DensityFront& fi = m_front[i];
        getFrontCellCenter((float* )&asamp._pos, fi);
		memcpy(&asamp._nml, fi._nml, 12);
		dst->push_back(asamp);
	}
}

}

#endif
