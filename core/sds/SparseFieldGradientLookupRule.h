/*
 *  SparseFieldGradientLookupRule.h
 *  gorse sds
 *
 *
 *  2019/7/13
 */

#ifndef ALO_SPARSE_FIELD_GRADIENT_LOOKUP_RULE_H
#define ALO_SPARSE_FIELD_GRADIENT_LOOKUP_RULE_H

#include "SparseFieldLookupRule.h"

namespace alo {
    
namespace sds {

template<typename Tc>
class SparseFieldGradientLookupRule : public SparseFieldLookupRule<Tc> {

public:

	SparseFieldGradientLookupRule(Tc *sfc);

/// i-th corner
	void computeCellCornerGradientKeys(int *gradKeys, int i) const;

protected:

private:

};

template<typename Tc>
SparseFieldGradientLookupRule<Tc>::SparseFieldGradientLookupRule(Tc *sfc) : SparseFieldLookupRule<Tc>(sfc)
{}

template<typename Tc>
void SparseFieldGradientLookupRule<Tc>::computeCellCornerGradientKeys(int *gradKeys, int i) const
{
	const int &nodeKey = cellCornerKeys()[i];
	int u[3];
	const Tc *sfc = c_curve();
	sfc->decodeKey(u, nodeKey);
	const int d = cellDelta();
	gradKeys[0] = sfc->computeKey(u[0] + d, u[1]    , u[2]);
	gradKeys[1] = sfc->computeKey(u[0] - d, u[1]    , u[2]);
	gradKeys[2] = sfc->computeKey(u[0]    , u[1] + d, u[2]);
	gradKeys[3] = sfc->computeKey(u[0]    , u[1] - d, u[2]);
	gradKeys[4] = sfc->computeKey(u[0]    , u[1]    , u[2] + d);
	gradKeys[5] = sfc->computeKey(u[0]    , u[1]    , u[2] - d);
}

}

}

#endif
