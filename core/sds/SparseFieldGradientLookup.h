/*
 *  SparseFieldGradientLookup.h
 *  gorse sds
 *
 *  grad(q) = (dq/dx, dq/dy, dq/dz)
 *
 *  2019/7/13
 */

#ifndef ALO_SPARSE_FIELD_GRADIENT_LOOKUP_H
#define ALO_SPARSE_FIELD_GRADIENT_LOOKUP_H

#include "SparseFieldLookup.h"

namespace alo {
    
namespace sds {

template<typename T>
class SparseFieldGradientLookup : public SparseFieldLookup<T> {

	typename T::ValueTyp m_curCellNodeGradients[3][8];

public:

	SparseFieldGradientLookup();
	virtual ~SparseFieldGradientLookup();

	template<typename Tr>
	void lookupGradient(const float *p, Tr &rule);

	void interpolateGradient(typename T::ValueTyp *grad, const float *p) const;

protected:

	template<typename Tr>
	void computeGradient(Tr &rule);

	template<typename Tr>
	typename T::ValueTyp computeSlope(const typename T::ValueTyp &v0,
								const int &k1, const int &k2, Tr &rule);

private:

};

template<typename T>
SparseFieldGradientLookup<T>::SparseFieldGradientLookup()
{}

template<typename T>
SparseFieldGradientLookup<T>::~SparseFieldGradientLookup()
{}

template<typename T>
template<typename Tr>
void SparseFieldGradientLookup<T>::lookupGradient(const float *p, Tr &rule)
{
	SparseFieldLookup<T>::lookup<Tr>(p, rule);
	if(SparseFieldLookup<T>::isStatusUpdate()) 
		computeGradient<Tr>(rule);
}

template<typename T>
template<typename Tr>
void SparseFieldGradientLookup<T>::computeGradient(Tr &rule)
{
	int gradKeys[6];
	int gradInds[6];
	for(int i=0;i<8;++i) {
		rule.computeCellCornerGradientKeys(gradKeys, i);

		for(int j=0;j<6;++j) {
			gradInds[j] = field()->getNodeIndex(gradKeys[j]);
		}

		const typename T::ValueTyp &v = SparseFieldLookup<T>::currentCellCornerValues()[i];

		m_curCellNodeGradients[0][i] = computeSlope<Tr>(v, gradInds[0], gradInds[1], rule);
		m_curCellNodeGradients[1][i] = computeSlope<Tr>(v, gradInds[2], gradInds[3], rule);
		m_curCellNodeGradients[2][i] = computeSlope<Tr>(v, gradInds[4], gradInds[5], rule);

	}

}

template<typename T>
template<typename Tr>
typename T::ValueTyp SparseFieldGradientLookup<T>::computeSlope(const typename T::ValueTyp &v0,
								const int &k1, const int &k2, Tr &rule)
{
	T::ValueTyp y = 0;
	if(k1 > -1 && k2 > -1) {
		y = (fieldValues()[k1] - fieldValues()[k2]) * currentCellInvSpan() * .5f;
	} else if(k1 > -1) {
		y = (fieldValues()[k1] - v0) * currentCellInvSpan();
	} else if(k2 > -1) {
		y = (v0 - fieldValues()[k2]) * currentCellInvSpan();
	}
	return y;
}

template<typename T>
void SparseFieldGradientLookup<T>::interpolateGradient(typename T::ValueTyp *grad, const float *p) const
{
	float wx = (p[0] - currentCellOriginSpan()[0]) * currentCellInvSpan();
	float wy = (p[1] - currentCellOriginSpan()[1]) * currentCellInvSpan();
	float wz = (p[2] - currentCellOriginSpan()[2]) * currentCellInvSpan();

	grad[0] = SparseFieldLookup<T>::Trilinear(m_curCellNodeGradients[0], wx, wy, wz);
	grad[1] = SparseFieldLookup<T>::Trilinear(m_curCellNodeGradients[1], wx, wy, wz);
	grad[2] = SparseFieldLookup<T>::Trilinear(m_curCellNodeGradients[2], wx, wy, wz);
}

}

}

#endif
