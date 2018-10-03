/* 
 * EllipseFunc.h
 * aloe
 *
 */
 
#ifndef ALO_ELLIPSE_FUNC_H
#define ALO_ELLIPSE_FUNC_H

namespace alo {
    
class EllipseFunc {
    
/// semi-major axis
    float m_a;
/// eccentricity 0 < e < 1
    float m_e;
    
public:

    EllipseFunc();
    
    void setA(float a);
    void setE(float e);

/// r by angular coordinate theta 
    float polarForm(float theta) const;
    
};
    
}

#endif
