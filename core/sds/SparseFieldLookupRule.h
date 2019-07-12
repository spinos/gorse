/*
 *  SparseFieldLookupRule.h
 *  gorse sds
 *
 *
 *  2019/7/13
 */

#ifndef ALO_SPARSE_FIELD_LOOKUP_RULE_H
#define ALO_SPARSE_FIELD_LOOKUP_RULE_H

#include "SparseFieldBuildRule.h"

namespace alo {
    
namespace sds {

template<typename Tc>
class SparseFieldLookupRule : public SparseFieldBuildRule<Tc> {

public:

	SparseFieldLookupRule(Tc *sfc);

protected:

private:

};

template<typename Tc>
SparseFieldLookupRule<Tc>::SparseFieldLookupRule(Tc *sfc) : SparseFieldBuildRule<Tc>(sfc)
{}

}

}

#endif
