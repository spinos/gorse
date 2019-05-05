/*
 *  LocalGridBuildRule.h
 *  gorse
 *
 *  2019/5/5
 */

#ifndef ALO_LOCAL_GRID_BUILD_RULE_H
#define ALO_LOCAL_GRID_BUILD_RULE_H

#include <svf/SvfBuildRule.h>

namespace alo {
    
namespace grd {
      
template<typename Tc>
class LocalGridBuildRule : public svf::SvfBuildRule<Tc> {

typedef svf::SvfBuildRule<Tc> SvfRuleTyp;

	float m_boundary;
	int m_P;

public:

    LocalGridBuildRule(Tc *sfc);

    void setP(int x);

    void setBoxRelativeBoundary(float x);

    void expandBox(float *b) const;

    const float &boundary() const;

    const int &P() const;
    
protected:

private:

};

template<typename Tc>
LocalGridBuildRule<Tc>::LocalGridBuildRule(Tc* sfc) : svf::SvfBuildRule<Tc>(sfc),
m_boundary(.1f),
m_P(4)
{}

template<typename Tc>
void LocalGridBuildRule<Tc>::setP(int x)
{ m_P = x; }

template<typename Tc>
void LocalGridBuildRule<Tc>::setBoxRelativeBoundary(float x)
{ m_boundary = x * SvfRuleTyp::deltaAtLevel(9); }

template<typename Tc>
void LocalGridBuildRule<Tc>::expandBox(float *b) const
{
	b[0] -= m_boundary;
	b[1] -= m_boundary;
	b[2] -= m_boundary;
	b[3] += m_boundary;
	b[4] += m_boundary;
	b[5] += m_boundary;
}

template<typename Tc>
const float &LocalGridBuildRule<Tc>::boundary() const
{ return m_boundary; }

template<typename Tc>
const int &LocalGridBuildRule<Tc>::P() const
{ return m_P; }

}

} /// end of namespace alo

#endif
