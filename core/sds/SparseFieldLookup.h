/*
 *  SparseFieldLookup.h
 *  gorse sds
 *
 *  sample sparse cubic field T
 *
 *  2019/7/13
 */

#ifndef ALO_SPARSE_FIELD_LOOKUP_H
#define ALO_SPARSE_FIELD_LOOKUP_H

namespace alo {
    
namespace sds {

template<typename T>
class SparseFieldLookup {
	
	T *m_field;
	typename T::ValueTyp m_curCellNodeValues[8];
	float m_cellOriginSpan[4];
	float m_cellInvSpan;
	int m_curCellKey;

	enum CacheStatus {
		sUnknown = 0,
		sUpdate,
		sCached
	};

	int m_status;

public:

	SparseFieldLookup();
	virtual ~SparseFieldLookup();

	void attach(T *field);
	void detach();

/// cache cell corner values and cell (origin, 1/size)
	template<typename Tr>
	void lookup(const float *p, Tr &rule);

	typename T::ValueTyp interpolate(const float *p) const;

	bool isStatusUnknown() const;
	bool isStatusUpdate() const;

protected:

	T *field();
	const typename T::ValueTyp *fieldValues() const;
	const typename T::ValueTyp *currentCellCornerValues() const;
	const float *currentCellOriginSpan() const;
	const float &currentCellInvSpan() const;

	static typename T::ValueTyp Trilinear(const typename T::ValueTyp *q, 
										const float &wx, const float &wy, const float &wz);

private:

};

template<typename T>
SparseFieldLookup<T>::SparseFieldLookup() : m_curCellKey(-1),
m_status(sUnknown)
{}

template<typename T>
SparseFieldLookup<T>::~SparseFieldLookup()
{}

template<typename T>
void SparseFieldLookup<T>::attach(T *field)
{ m_field = field; }

template<typename T>
void SparseFieldLookup<T>::detach()
{ 
	m_curCellKey = -1; 
	m_status = sUnknown;
}

template<typename T>
template<typename Tr>
void SparseFieldLookup<T>::lookup(const float *p, Tr &rule)
{
	const int cellKey = rule.computeKey(p);
	if(m_curCellKey == cellKey) {
		m_status = sCached;
		return;
	}

	m_curCellKey = -1;

	rule.computeCellCornerKeys(cellKey);

	int cornerInd[8];
	for(int i=0;i<8;++i) {
		cornerInd[i] = m_field->getNodeIndex(rule.cellCornerKeys()[i]);
		if(cornerInd[i] < 0) {
			m_status = sUnknown;
			return;
		}
	}

	for(int i=0;i<8;++i) {
		m_curCellNodeValues[i] = fieldValues()[cornerInd[i]];
	}

	rule.computePosition(m_cellOriginSpan, cellKey);
	m_cellOriginSpan[3] = rule.getCellSize();
	m_cellInvSpan = 1.f / m_cellOriginSpan[3];

	m_curCellKey = cellKey;
	m_status = sUpdate;
}

template<typename T>
typename T::ValueTyp SparseFieldLookup<T>::interpolate(const float *p) const
{
	float wx = (p[0] - m_cellOriginSpan[0]) * m_cellInvSpan;
	float wy = (p[1] - m_cellOriginSpan[1]) * m_cellInvSpan;
	float wz = (p[2] - m_cellOriginSpan[2]) * m_cellInvSpan;

	return Trilinear(m_curCellNodeValues, wx, wy, wz);
}

template<typename T>
typename T::ValueTyp SparseFieldLookup<T>::Trilinear(const typename T::ValueTyp *q, 
												const float &wx, const float &wy, const float &wz)
{	
	T::ValueTyp a = q[0] * (1.f - wx) + q[1] * wx;
	T::ValueTyp b = q[2] * (1.f - wx) + q[3] * wx;
	T::ValueTyp c = q[4] * (1.f - wx) + q[5] * wx;
	T::ValueTyp d = q[6] * (1.f - wx) + q[7] * wx;

	a = a * (1.f - wy) + b * wy;
	c = c * (1.f - wy) + d * wy;
	
	return a * (1.f - wz) + c * wz;
}

template<typename T>
const float *SparseFieldLookup<T>::currentCellOriginSpan() const
{ return m_cellOriginSpan; }

template<typename T>
const float &SparseFieldLookup<T>::currentCellInvSpan() const
{ return m_cellInvSpan; }

template<typename T>
T *SparseFieldLookup<T>::field()
{ return m_field; }

template<typename T>
const typename T::ValueTyp *SparseFieldLookup<T>::fieldValues() const
{ return m_field->c_values(); }

template<typename T>
const typename T::ValueTyp *SparseFieldLookup<T>::currentCellCornerValues() const
{ return m_curCellNodeValues; }

template<typename T>
bool SparseFieldLookup<T>::isStatusUnknown() const
{ return m_status == sUnknown; }

template<typename T>
bool SparseFieldLookup<T>::isStatusUpdate() const
{ return m_status == sUpdate; }

}

}

#endif
