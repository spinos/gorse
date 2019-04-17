/*
 *  SvField.h
 *  aloe
 *
 *  sparse voxel field of unknown P, Q, data_type
 *  
 *  Created by jian zhang on 3/4/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_SV_FIELD_H
#define ALO_SV_FIELD_H

#include <sds/CubicField.h>
#include <boost/scoped_array.hpp>

namespace alo {

namespace svf {

class SvField {

	boost::scoped_array<char> m_lpValue;
/// to (1<<P)^3 level p cells
	boost::scoped_array<int> m_cellIndices;
/// (origin, cell_size, 1/cell_size, 1/q_cell_size)
	float m_coord[6];
/// (1<<P, 1<<(Q-P), (1<<P)-1, (1<<(Q-P))-1)
	int m_dim[4];
	int m_P, m_Q, m_lpSize;
	
public:

	SvField();
	~SvField();
	
	void create(int p, int q, int lpDistanceDataSize);
/// (origin, cell_size)
	void setOriginCellSize(const float* v);
	
	const int& P() const;
	const int& Q() const;
	const float* originCellSize() const;
	int cellIndLength() const;
	const int& valueStorageSize() const;
	const char* c_value() const;
	const int* c_cellIndValue() const;
	
	int* cellIndValue();
	char* value();
	
	void copyCellOffset(const int* b);
	void copyCellValue(const int offset, const char* b, const int dataSize);
	
	void getCellValue(char* b, int size, int i, int j, int k) const;
/// minmax box as (x0,y0,z0,x1,y1,z1)
	void getBox(float* b) const;
	void getCellBox(float* b, int i, int j, int k) const;
/// (center, half_span)
	void getCoord(float* c) const;
	void getOriginCellSize(float* b) const;
	const float& cellSize() const;
	float delta() const;
	const int& numCellsPerDimension() const;
	const int& numCellDivisionPerDimension() const;
	bool isCellEmpty(int i, int j, int k) const;
	
/// at p
	template<typename T>
	T lookup(const float* p) const;
	
protected:
	
	void computeCellCoord(int* u, const float* p) const;
	int computeCellInd(const int* u) const;
	
	template<typename T>
	void lookupInCell(T& dst, const float* p, const int* u,
					const T* v) const;
	
private:

	void computeCellWeight(float& wx, float& wy, float& wz,
						int& i, int& j, int& k,
						const float* p, const int* u) const;
	int cellValueInd(int i, int j, int k) const;

};

template<typename T>
T SvField::lookup(const float* p) const
{
	T res;
	int u[3];
	computeCellCoord(u, p);
	const int c = computeCellInd(u);
	const int offset = c_cellIndValue()[c];
	if(offset > -1) {
		const T* fv = (const T*)&c_value()[offset];
		lookupInCell<T>(res, p, u, fv);
	} else {
		std::cout<<"\n empty cell"<<u[0]<<","<<u[1]<<","<<u[2];
	}
	return res;
}

template<typename T>
void SvField::lookupInCell(T& dst, const float* p, const int* u,
					const T* v) const
{
	float wx, wy, wz;
	int i, j, k;
	computeCellWeight(wx, wy, wz, i, j, k, p, u);
	
	T a = v[cellValueInd(i  ,j  ,k)] * (1.f - wx)
			+ v[cellValueInd(i+1,j  ,k)] * wx;
	T b = v[cellValueInd(i  ,j+1,k)] * (1.f - wx)
			+ v[cellValueInd(i+1,j+1,k)] * wx;
	T c = v[cellValueInd(i  ,j  ,k+1)] * (1.f - wx)
			+ v[cellValueInd(i+1,j  ,k+1)] * wx;
	T d = v[cellValueInd(i  ,j+1,k+1)] * (1.f - wx)
			+ v[cellValueInd(i+1,j+1,k+1)] * wx;
	
	a = a * (1.f - wy) + b * wy;
	c = c * (1.f - wy) + d * wy;
	
	dst = a * (1.f - wz) + c * wz;
}

}

}

#endif
