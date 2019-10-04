/*
 *  Spherical.h
 *  gorse
 *
 *  r :	Radial distance
 *  phi	: Zenith angle (0<= phi <= pi)
 *  theta : Azimuth angle (0<= theta <= 2pi)
 *
 *  2019/9/30
 */
 
#ifndef ALO_MATH_SPHEREICAL_H
#define ALO_MATH_SPHEREICAL_H

namespace alo {

class Vector3F;

class Spherical {
    
public:

    static Vector3F toCartesian(const float &r, const float &phi, const float &theta);
    
};
    
}

#endif