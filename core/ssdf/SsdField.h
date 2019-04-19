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
#include <math/Vector3F.h>

namespace alo {

namespace sdf {

class SsdField {

	sds::CubicField<float> m_coarseDistance;
	boost::scoped_array<float> m_fineDistance;
    sds::CubicField<Vector3F> m_coarseNormal;
    boost::scoped_array<Vector3F> m_fineNormal;
/// to (1<<P)^3 level p cells
	boost::scoped_array<int> m_cellIndices;
	int m_P, m_Q, m_fltStorageSize;
	
public:

	SsdField();
	~SsdField();
	
	void create(int p, int q, int fltStorageSize);
    void destroy();
/// (origin, cell_size)
	void setOriginCellSize(const float* v);
	
    bool isEmpty() const;
	const int& P() const;
	const int& Q() const;
	const float* originCellSize() const;
	int coarseDistanceStorageSize() const;
	int coarseNormalStorageSize() const;
	const float* c_coarseDistanceValue() const;
	const int& fineDistanceStorageSize() const;
	const float* c_fineDistanceValue() const;
	const Vector3F *c_coarseNormalValue() const;
	int fineNormalStorageSize() const;
	const Vector3F *c_fineNormalValue() const;
	int cellIndLength() const;
	const int* c_cellIndValue() const;
	
	int* cellIndValue();
	float* coarseDistanceValue();
	float* fineDistanceValue();
	Vector3F *coarseNormalValue();
	Vector3F *fineNormalValue();
	
	void copyCoarseDistanceValue(const sds::CubicField<float>* b);
	void copyFineDistanceValue(const int offset, const sds::CubicField<float>* b);
	void copyCellOffset(const int* b);
    
    void copyCoarseNormalValue(const sds::CubicField<Vector3F>* b);
	void copyFineNormalValue(const int offset, const sds::CubicField<Vector3F>* b);
	
/// minmax box as (x0,y0,z0,x1,y1,z1)
	void getBox(float* b) const;
/// (center, half_span)
	void getCoord(float* c) const;
	void getOriginCellSize(float* b) const;
	const float& cellSize() const;
	float delta() const;
/// at u
	float lookup(const float* u) const;
    Vector3F lookupNormal(const float* u) const;

	int totalStorageSize() const;
    
protected:

private:

};

}

}

#endif
