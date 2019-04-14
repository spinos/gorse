/*
 *  IntersectResult.h
 *  aloe
 *
 *  Created by zhang on 18-2-22.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_INTERSECT_RESULT_H
#define ALO_INTERSECT_RESULT_H

#include <math/Vector3F.h>

namespace alo {

class SampleState;

class IntersectResult {
    
	SampleState* m_radianceState;
    Vector3F m_normal;
    float m_dist;
    int m_materialId;
    int m_lightId;
    
public:
	
	IntersectResult();

/// dist <- x, normal <- nml when x < dist
	bool updateRayDistance(float x, const Vector3F &nml);
	
	float& rayDistance();
	Vector3F& hitNormal();
	SampleState* radianceState();
	
};

}

#endif