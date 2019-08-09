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

	const int n = 1 + span / m_pCellSpan;
	Vector3F q;
	for(int k=0;k<=n;++k) {
		if(k<=n-1) q.z = origin.z + m_pCellSpan * k;
		else q.z = origin.z + span;

		for(int j=0;j<=n;++j) {
			if(j<=n-1) q.y = origin.y + m_pCellSpan * j;
			else q.y = origin.y + span;

			for(int i=0;i<=n;++i) {
				if(i<=n-1) q.x = origin.x + m_pCellSpan * i;
				else q.x = origin.x + span;

				if(SvfRuleTyp::isPointInsideDomain(q))
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
