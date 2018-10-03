/* 
 * EllipseFunc.cpp
 * aloe
 *
 */
 
#include "EllipseFunc.h"
#include <math.h>

namespace alo {
 
EllipseFunc::EllipseFunc() :
    m_a(1.f), m_e(.9f)
{}
    
void EllipseFunc::setA(float a)
{ m_a = a; }

void EllipseFunc::setE(float e)
{ m_e = e; }

float EllipseFunc::polarForm(float theta) const
{
    float b = sqrt( (1.f - m_e * m_e) * m_a * m_a ); 
    float ec = m_e * cos(theta);
    return b / sqrt(1.f - ec * ec);
}

}
