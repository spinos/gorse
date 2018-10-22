/* 
 * EllipseFunc.cpp
 * aloe
 *
 */
 
#include "EllipseFunc.h"
#include <math.h>

namespace alo {
 
EllipseFunc::EllipseFunc() :
    m_a(1.f), m_e(.1f), m_aasx(true)
{}
    
void EllipseFunc::setA(float a)
{ 
    m_a = a; 
    m_aasx = true;
}

void EllipseFunc::setE(float e)
{ m_e = e; }

void EllipseFunc::setB(float b)
{
    if(b<=m_a) {
        m_e = sqrt(1.f - (b*b)/(m_a*m_a));
    } else {
        m_e = sqrt(1.f - (m_a*m_a)/(b*b));
        m_a = b;
        m_aasx = false;
    }
    if(m_e > .8f) m_e = .8f;
}

float EllipseFunc::polarForm(float theta) const
{
    float b = sqrt( (1.f - m_e * m_e) * m_a * m_a ); 
    
    float phi = theta;
    if(!m_aasx) phi += 1.57f;
    float ec = m_e * cos(phi);
    return b / sqrt(1.f - ec * ec);
}

}
