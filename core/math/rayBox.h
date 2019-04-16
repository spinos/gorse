/*
 *  rayBox.h
 *  aloe
 *
 *  Created by zhang on 18-2-19.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_MATH_RAY_BOX_H
#define ALO_MATH_RAY_BOX_H

namespace alo {
    
/// ray is (origin, direction, t_min, t_max)
/// box is [-1,1]
inline bool rayOneBoxIntersect(float* ray)
{
    float t0 = ray[6];
    float t1 = ray[7];
    float tmp;
    
    for (int i=0; i<3; ++i) {
    
		const float& diri = ray[3+i];
		
		if(diri < 1e-8f && diri > -1e-8f) {
			if(ray[i] < -1.f || ray[i] > 1.f) {
                return false;
            }
			
			continue;
		}
/// Update interval for _i_th bounding box slab
        float invRayDir = 1.f / diri;
        float tNear = (-1.f - ray[i]) * invRayDir;
        float tFar  = (1.f - ray[i]) * invRayDir;

/// Update parametric interval from slab intersection $t$s
        if (tNear > tFar) {
            tmp = tNear;
            tNear = tFar;
            tFar = tmp;
        }
        
        t0 = tNear > t0 ? tNear : t0;
        t1 = tFar  < t1 ? tFar  : t1;
        
        if (t0 > t1) 
            return false;
    }
    
/// update ray
    ray[6] = t0;
    ray[7] = t1;
    
    return true;

}

/// ray is (origin, direction, t_min, t_max)
/// box is (center, half_span)
inline bool rayBoxIntersect(float* ray, const float* box)
{
    float t0 = ray[6];
    float t1 = ray[7];
    float tmp;
    
    const float& h = box[3];
    
    for (int i=0; i<3; ++i) {
    
		const float& diri = ray[3+i];
		
		if(diri < 1e-8f && diri > -1e-8f) {
			if(ray[i] < box[i] - h || ray[i] > box[i] + h) {
                return false;
            }
			
			continue;
		}
/// Update interval for _i_th bounding box slab
        float invRayDir = 1.f / diri;
        float tNear = (box[i] - h - ray[i]) * invRayDir;
        float tFar  = (box[i] + h - ray[i]) * invRayDir;

/// Update parametric interval from slab intersection $t$s
        if (tNear > tFar) {
            tmp = tNear;
            tNear = tFar;
            tFar = tmp;
        }
        
        t0 = tNear > t0 ? tNear : t0;
        t1 = tFar  < t1 ? tFar  : t1;
        
        if (t0 > t1) 
            return false;
    }
    
/// update ray
    ray[6] = t0;
    ray[7] = t1;
    
    return true;

}

inline bool rayExitingBox(const float* p,
                    const float* ray, const float* box)
{
    if(ray[3] < 0 && p[0] <= box[0] - box[3])
        return true;
        
    if(ray[3] > 0 && p[0] >= box[0] + box[3])
        return true;
        
    if(ray[4] < 0 && p[1] <= box[1] - box[3])
        return true;
        
    if(ray[4] > 0 && p[1] >= box[1] + box[3])
        return true;
        
    if(ray[5] < 0 && p[2] <= box[2] - box[3])
        return true;
        
    if(ray[5] > 0 && p[2] >= box[2] + box[3])
        return true;
        
    return false;
}

inline void getNormalOnBox(float* nml, const float* pnt, const float* box, const float delta)
{
    if(pnt[0] < box[0] - box[3] + delta) {
        nml[0] = -1.f;
        nml[1] = nml[2] = 0.f;
        return;
    }

    if(pnt[0] > box[0] + box[3] - delta) {
        nml[0] = 1.f;
        nml[1] = nml[2] = 0.f;
        return;
    }

    if(pnt[1] < box[1] - box[3] + delta) {
        nml[1] = -1.f;
        nml[0] = nml[2] = 0.f;
        return;
    }

    if(pnt[1] > box[1] + box[3] - delta) {
        nml[1] = 1.f;
        nml[0] = nml[2] = 0.f;
        return;
    }

    if(pnt[2] < box[2] - box[3] + delta) {
        nml[2] = -1.f;
        nml[0] = nml[1] = 0.f;
        return;
    }

    nml[2] = 1.f;
    nml[0] = nml[1] = 0.f;

}

}

#endif
