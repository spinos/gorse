/*
 *  LegendreBuildRule.h
 *  aloe
 *
 *  Created by zhang on 18-2-24.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_SDS_LEGENDRE_BUILD_RULE_H
#define ALO_SDS_LEGENDRE_BUILD_RULE_H

#include "SvtBuildRule.h"
#include <math/LegendreInterpolation.h>
#include "FrontDistanceMeasure.h"
#include <vector>

namespace alo {

namespace sds {

template<typename T, int P>
struct Coeff3 {
    T _v[P*P*P];
};

struct LegendreSdfNode {
    Coeff3<float, 4> _coeff;
	static int DataSize()
	{ return 256; }
};

template<typename Tc, typename Ts>
class LegendreBuildRule : public SvtBuildRule<Tc> {

    typedef LegendreInterpolation<float, 4, 3> PolyInterpTyp;
    PolyInterpTyp* m_poly;
    float m_Yijk[64];
    
typedef LegendreSdfNode NodeTyp;
    NodeTyp m_Coeijk;
    
    std::vector<NodeTyp> m_coeffs;
    
    FrontDistanceMeasure<Ts> m_distance;
    
public:
    
    LegendreBuildRule(const Tc& sfc);
    ~LegendreBuildRule();
    
    template<typename Tn>
    void buildLeafNode(Tn& node,
                const SpaceFillingVector<Ts>& samples, 
				const SpaceFillingVector<Ts>& fronts);

    const NodeTyp& data(int i) const;
    int numNodes() const;
    
protected:

private:
    
    void estimateError() const;
    
};

template<typename Tc, typename Ts>
LegendreBuildRule<Tc, Ts>::LegendreBuildRule(const Tc& sfc) : SvtBuildRule<Tc>(sfc)
{
    PolyInterpTyp::Initialize();
    m_poly = new PolyInterpTyp;
}

template<typename Tc, typename Ts>
LegendreBuildRule<Tc, Ts>::~LegendreBuildRule()
{
    delete m_poly;
    m_coeffs.clear();
}

template<typename Tc, typename Ts>
template<typename Tn>
void LegendreBuildRule<Tc, Ts>::buildLeafNode(Tn& node,
                const SpaceFillingVector<Ts>& samples, 
				const SpaceFillingVector<Ts>& fronts)
{
    int frontRange[2];
    fronts.searchSFC(frontRange, node._key, node._nextKey);
    m_distance.attach(node._coord, fronts, frontRange, samples, node._range);
    m_poly-> template compute3 <FrontDistanceMeasure<Ts> > (m_Yijk, m_Coeijk._coeff._v, m_distance );	
/// ind to data
    node._dataInd = m_coeffs.size();
    m_coeffs.push_back(m_Coeijk);
    //estimateError();
}

template<typename Tc, typename Ts>
void LegendreBuildRule<Tc, Ts>::estimateError() const
{
    float err, mxErr = 0.f, sumErr = 0.f;
	const float du = 2.f / 8;
    float u[3];
    for(int k=0;k<8;++k) {
		u[2] = -1.f + du * (.5f + k);
		for(int j=0;j<8;++j) {
			u[1] = -1.f + du * (.5f + j);
			for(int i=0;i<8;++i) {
				u[0] = -1.f + du * (.5f + i);
				
				float m_exact = m_distance.measureAt(u[0], u[1], u[2]);
				float m_appro = m_poly->Approximate3(u, m_Coeijk );
				
				err = Absolute<float>(m_appro - m_exact);
				if(mxErr < err)
					mxErr = err;
				sumErr += err;
			}
		}
	}
	
	std::cout<<"\n max error "<<mxErr<<" average "<<(sumErr/512);
}

template<typename Tc, typename Ts>
const LegendreSdfNode& LegendreBuildRule<Tc, Ts>::data(int i) const
{ return m_coeffs[i]; }

template<typename Tc, typename Ts>
int LegendreBuildRule<Tc, Ts>::numNodes() const
{ return m_coeffs.size(); }

}

}

#endif
