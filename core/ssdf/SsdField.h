/*
 *  SsdField.h
 *  aloe
 *
 *  sparse signed distance field of unknown P, Q, data_type
 *  
 *  Created by jian zhang on 3/4/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_SSD_FIELD_H
#define ALO_SSD_FIELD_H

#include <sds/CubicField.h>
#include <boost/scoped_array.hpp>

namespace alo {

namespace sdf {

class SsdField {

	sds::CubicField<float> m_coarseDistance;
	boost::scoped_array<float> m_lpDistance;
/// to (1<<P)^3 level p cells
	boost::scoped_array<int> m_cellIndices;
	int m_P, m_Q, m_lpSize;
	
public:

	SsdField();
	~SsdField();
	
	void create(int p, int q, int lpDistanceDataSize);
/// (origin, cell_size)
	void setOriginCellSize(const float* v);
	
	const int& P() const;
	const int& Q() const;
	const float* originCellSize() const;
	int coarsDistanceStorageSize() const;
	const float* c_coarseDistanceValue() const;
	const int& fineDistanceStorageSize() const;
	const float* c_fineDistanceValue() const;
	int cellIndLength() const;
	const int* c_cellIndValue() const;
	
	int* cellIndValue();
	float* coarseDistanceValue();
	float* fineDistanceValue();
	
	void copyCoarseDistanceValue(const sds::CubicField<float>* b);
	void copyFineDistanceValue(const int offset, const sds::CubicField<float>* b);
	void copyCellOffset(const int* b);
	
/// minmax box as (x0,y0,z0,x1,y1,z1)
	void getBox(float* b) const;
/// (center, half_span)
	void getCoord(float* c) const;
	void getOriginCellSize(float* b) const;
	const float& cellSize() const;
	float delta() const;
/// at u
	float lookup(const float* u) const;
	
protected:

private:

};

}

}

#endif
