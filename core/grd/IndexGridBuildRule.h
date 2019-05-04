/*
 *  IndexGridBuildRule.h
 *  gorse
 *
 *  2019/5/4
 */

#ifndef INDEX_GRID_BUILD_RULE_H
#define INDEX_GRID_BUILD_RULE_H

#include <svf/SvfBuildRule.h>

namespace alo {
    
namespace grd {
      
template<typename Tc>
class IndexGridBuildRule : public svf::SvfBuildRule<Tc> {

typedef svf::SvfBuildRule<Tc> SvfRuleTyp;

	float m_boundary;

public:

    IndexGridBuildRule(Tc *sfc);

    void setBoxRelativeBoundary(float x);

    void expandBox(float *b) const;

    const float &boundary() const;
    
protected:

private:

};

template<typename Tc>
IndexGridBuildRule<Tc>::IndexGridBuildRule(Tc* sfc) : svf::SvfBuildRule<Tc>(sfc)
{ m_boundary = 0.1f; }

template<typename Tc>
void IndexGridBuildRule<Tc>::setBoxRelativeBoundary(float x)
{ m_boundary = x * SvfRuleTyp::deltaAtLevel(9); }

template<typename Tc>
void IndexGridBuildRule<Tc>::expandBox(float *b) const
{
	b[0] -= m_boundary;
	b[1] -= m_boundary;
	b[2] -= m_boundary;
	b[3] += m_boundary;
	b[4] += m_boundary;
	b[5] += m_boundary;
}

template<typename Tc>
const float &IndexGridBuildRule<Tc>::boundary() const
{ return m_boundary; }

}

} /// end of namespace alo

#endif
