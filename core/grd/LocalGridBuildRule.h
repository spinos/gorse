/*
 *  LocalGridBuildRule.h
 *  gorse
 *
 *  2019/8/10
 */

#ifndef ALO_LOCAL_GRID_BUILD_RULE_H
#define ALO_LOCAL_GRID_BUILD_RULE_H

#include <svf/SvfBuildRule.h>
#include <vector>

namespace alo {
    
namespace grd {
      
template<typename Tc>
class LocalGridBuildRule : public svf::SvfBuildRule<Tc> {

typedef svf::SvfBuildRule<Tc> SvfRuleTyp;

	int m_P;
	float m_pCellSpan;
	std::vector<int> m_cellKeys;

public:

    LocalGridBuildRule(Tc *sfc);

    void setP(int x);

    const int &P() const;

    int computeIntersectedCellKeys(const Vector3F &origin, 
    							const float &span);
    const int &intersectedCellKey(const int &i) const;

protected:

private:

};

template<typename Tc>
LocalGridBuildRule<Tc>::LocalGridBuildRule(Tc* sfc) : svf::SvfBuildRule<Tc>(sfc),
m_P(4)
{}

template<typename Tc>
void LocalGridBuildRule<Tc>::setP(int x)
{ 
	m_P = x;
	m_pCellSpan = deltaAtLevel(x);
}

template<typename Tc>
const int &LocalGridBuildRule<Tc>::P() const
{ return m_P; }

template<typename Tc>
int LocalGridBuildRule<Tc>::computeIntersectedCellKeys(const Vector3F &origin, 
    							const float &span)
{
	m_cellKeys.clear();
    
    Vector3F p0 = origin;
    Vector3F p1 = origin + Vector3F(span, span, span);
    SvfRuleTyp::clipBoxInsideDomain(p0, p1);
    
    float d0[3];
    SvfRuleTyp::getDomainOrigin(d0);
    int u0[3];
    u0[0] = (p0.x - d0[0]) / m_pCellSpan;
    u0[1] = (p0.y - d0[1]) / m_pCellSpan;
    u0[2] = (p0.z - d0[2]) / m_pCellSpan;
    int u1[3];
    u1[0] = 1 + (p1.x - d0[0]) / m_pCellSpan;
    u1[1] = 1 + (p1.y - d0[1]) / m_pCellSpan;
    u1[2] = 1 + (p1.z - d0[2]) / m_pCellSpan;

	const int n = 1 + span / m_pCellSpan;
	Vector3F q;
	for(int k=u0[2];k<u1[2];++k) {
		q.z = d0[2] + m_pCellSpan * k;

		for(int j=u0[1];j<u1[1];++j) {
			q.y = d0[1] + m_pCellSpan * j;

			for(int i=u0[0];i<u1[0];++i) {
				q.x = d0[0] + m_pCellSpan * i;

				m_cellKeys.push_back(SvfRuleTyp::computeKeyAtLevel((const float *)&q, P()));
			}
		}
	}
	return m_cellKeys.size();
}

template<typename Tc>
const int &LocalGridBuildRule<Tc>::intersectedCellKey(const int &i) const
{ return m_cellKeys[i]; }

}

} /// end of namespace alo

#endif
