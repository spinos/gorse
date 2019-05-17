/*
 *  InstanceRecord.h
 *  gorse
 *
 *  tm and id of n instances
 *
 *  2019/5/17
 */

#ifndef ALO_INSTANCE_RECORD_H
#define ALO_INSTANCE_RECORD_H

#include <math/SimpleBuffer.h>
#include <math/Matrix44F.h>
#include <math/BoundingBox.h>

namespace alo {

namespace grd {

class InstanceRecord {

	SimpleBuffer<Matrix44F > m_tms;
	SimpleBuffer<int> m_inds;
    BoundingBox m_bbox;
	int m_numInstances;
	int m_numObjects;

public:

	InstanceRecord();
	~InstanceRecord();

	void create(const int &numObjects, const int &numInstances);
    void setAabb(const float *b);
/// after set tms
    void calcAabb();

	Matrix44F *tms();
	int *inds();

	const int &numInstances() const;
	const int &numObjects() const;

	const Matrix44F *c_tms() const;
	const int *c_inds() const;
    
    bool isEmpty() const;
    void getAabb(float *b) const;
/// to limit number of cells
    float getMinimumCellSize() const;

	void verbose() const;

protected:

private:

};

} /// end of grd

} /// end of alo

#endif
