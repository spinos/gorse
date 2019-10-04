/*
 *  Spherical.cpp
 *  gorse
 *
 *  2019/9/30
 */
 
#include "Spherical.h"
#include "Vector3F.h"
#include <cmath>

namespace alo {

Vector3F Spherical::toCartesian(const float &r, const float &phi, const float &theta)
{
    float sphi = sin(phi);
    float cphi = cos(phi);
    float stheta = sin(theta);
    float ctheta = cos(theta);
    return Vector3F(r * sphi * ctheta,
                    r * sphi * stheta,
                    r * cphi);
}
    
}
