/*
 *  LegendreTraverseRule.h
 *  aloe
 *
 *  Tc is sfc
 *  T is node
 *  N is subdiv order
 *
 *  Created by zhang on 18-2-25.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_SDS_LEGENDRE_TRAVERSE_RULE_H
#define ALO_SDS_LEGENDRE_TRAVERSE_RULE_H

#include "SvtTraverseRule.h"
#include <math/rayBox.h>
#include <math/Vector3F.h>
#include <math/LegendreInterpolation.h>

namespace alo {

namespace sds {

template<typename Tc, typename T, int N>
class LegendreTraverseRule : public SvtTraverseRule<Tc, N> {

    Vector3F m_normal;    
    float m_rayDepth;
    
typedef LegendreInterpolation<float, 4, 3> PolyInterpTyp;
	
public:
    
    LegendreTraverseRule();
    
    bool intersectLeafCell(const float* ray,
                        const float* coord, 
                        const T& node);
    
private:

};

template<typename Tc, typename T, int N>
LegendreTraverseRule<Tc, T, N>::LegendreTraverseRule()
{}

template<typename Tc, typename T, int N>
bool LegendreTraverseRule<Tc, T, N>::intersectLeafCell(const float* ray,
                                        const float* coord,
                                        const T& node)
{
    if(SvtTraverseRule<Tc, N>::isInnerCell(coord[3]))
        return false;
    
    const float invscal = 1.f / coord[3];
    float localRay[8];
    memcpy(localRay, ray, 32);
    localRay[0] -= coord[0];
    localRay[1] -= coord[1];
    localRay[2] -= coord[2];
    localRay[0] *= invscal;
    localRay[1] *= invscal;
    localRay[2] *= invscal;
    localRay[6] *= invscal;
    localRay[7] *= invscal;
    
    if(!rayOneBoxIntersect(localRay) ) 
        return false;
    
    float u[3];
    u[0] = localRay[0] + localRay[3] * localRay[6];
    u[1] = localRay[1] + localRay[4] * localRay[6];
    u[2] = localRay[2] + localRay[5] * localRay[6];
    
    const float* coeff = node._coeff._v;
    float fd = PolyInterpTyp::Approximate3(u, coeff);
    
    int step = 0;
	while(fd > .1f) {
/// limit step size
        if(fd < .1f) fd = .1f;
		//fd *= .5f;
        
		u[0] += localRay[3] * fd;
		u[1] += localRay[4] * fd;
		u[2] += localRay[5] * fd;
		
        localRay[6] += fd;
		if(localRay[6] > localRay[7])
			return false;
			
		fd = PolyInterpTyp::Approximate3(u, coeff);

		step++;
		if(step > 40)
			return false;
	}
	
	PolyInterpTyp::ApproximateGradient3((float* )&m_normal, fd, 
	                                        u, coeff);
	m_normal.normalize();
	
/// depth in world
	m_rayDepth = localRay[6] * coord[3];
    return true;
}

}

}
#endif

