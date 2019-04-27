/*
 *  IndexGridBuildRule.h
 *  gorse
 *
 *  2019/4/28
 */

#ifndef INDEX_GRID_BUILD_RULE_H
#define INDEX_GRID_BUILD_RULE_H

#include <svf/SvfBuildRule.h>

namespace alo {
    
namespace grd {
      
template<typename Tc>
class IndexGridBuildRule : public svf::SvfBuildRule<Tc> {

typedef svf::SvfBuildRule<Tc> SvfRuleTyp;

public:

    IndexGridBuildRule(Tc *sfc);
    
protected:

private:

};

template<typename Tc>
IndexGridBuildRule<Tc>::IndexGridBuildRule(Tc* sfc) : svf::SvfBuildRule<Tc>(sfc)
{}

}

} /// end of namespace alo

#endif
