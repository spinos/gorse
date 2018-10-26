/* 
 * YCylinder.h
 * aloe
 *
 * cylinder shape along y axis
 *
 */

#ifndef X_ARROW_H
#define X_ARROW_H

#include "ATriangleMesh.h"

namespace alo {

class YCylinder : public ATriangleMesh {

/// num cells u and v
	int m_nu, m_nv;
    float m_radius, m_circum, m_height;

public:
    YCylinder();
    
    void createCylinder(int nu, int nv, float radius, float height);
    
private:

    void addOddCell(unsigned* ind, int& tri,
				const int& i, const int& j,
				const int& i1, const int& j1,
				const int& nu);
	void addEvenCell(unsigned* ind, int& tri,
				const int& i, const int& j,
				const int& i1, const int& j1,
				const int& nu);
};

}

#endif