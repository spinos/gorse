/*
 *  SsdField.h
 *  gorse
 *
 *  sparse signed distance field of unknown P, Q
 *  coarse distance stored in field values
 *  fine data offset stored in grid cells
 *
 *  2019/8/10
 */

#ifndef ALO_SSD_FIELD_H
#define ALO_SSD_FIELD_H

#include <sds/CubicGrid.h>
#include <sds/SpaceFillingVector.h>
#include <math/pointBox.h>

namespace alo {

namespace sdf {

class SsdField : public sds::CubicGrid<float, int> {

	typedef sds::CubicGrid<float, int> PGridTyp;
	SimpleBuffer<float> m_fineDistance;
    sds::CubicField<Vector3F> m_coarseNormal;
    SimpleBuffer<Vector3F> m_fineNormal;
	int m_P, m_Q, m_numFineValues;
	BoundingBox m_bbox;
	
public:

	SsdField();
	virtual ~SsdField();
	
	void create(int p, int q, int l);
    void destroy();
/// (origin, cell_size)
	void setOriginCellSize(const float* v);
	void setBBox(const BoundingBox &b);
	void setAabb(const float *b);
	
    bool isEmpty() const;
	const int& P() const;
	const int& Q() const;
	const int &numFineValues() const;

	const float* c_fineDistanceValue() const;
	const Vector3F *c_coarseNormalValue() const;
	const Vector3F *c_fineNormalValue() const;

	float* fineDistanceValue();
	Vector3F *coarseNormalValue();
	Vector3F *fineNormalValue();
	
	void copyCoarseNormalValue(const sds::CubicField<Vector3F>* b);
	void copyFineDistanceValue(const int offset, const sds::CubicField<float>* b);
    void copyFineNormalValue(const int offset, const sds::CubicField<Vector3F>* b);
	
/// Q cell size
	float delta() const;

    Vector3F lookupNormal(const float* u) const;

/// b is float[6] in (low, high)
	void getAabb(float *b) const;
	void expandAabb(float *b) const;
	const float *aabb() const;

/// nonempty p cells
	template<typename T>
	void genSamples(sds::SpaceFillingVector<T> &samples) const;
	
	void verbose() const;
    
protected:

private:

};

template<typename T>
void SsdField::genSamples(sds::SpaceFillingVector<T> &samples) const
{
	T ap;
    ap._span = cellSize();
	BoundingBox b;
	const int n = numCells();
	for(int i=0;i<n;++i) {
		const int offset = c_cell()[i];
		if(offset < 0) continue;

		getCellBox(b, i);
		ap._pos = b.center();
		
		samples<<ap;
	}
}

}

}

#endif
