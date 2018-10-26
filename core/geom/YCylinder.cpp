/* 
 * YCylinder.cpp
 * aloe
 *
 */

#include "YCylinder.h"

namespace alo {

YCylinder::YCylinder()
{}

void YCylinder::createCylinder(int nu, int nv, float radius, float height)
{
    const int np = nu * (nv + 1);
    const int nt = nu * nv * 2;
    createTriangleMesh(np, nt);
    
    Vector3F * p = positions();
    unsigned * ind = indices();
    
    const float da = 6.28318531f / (float)nu;
	m_circum = Vector3F(cos(da), 0.f, sin(da) ).distanceTo(Vector3F::XAxis) * radius * nu; 
    const float du = m_circum / (float)nu;
    const float dv = height / (float)nv;
    
    int acc = 0;
	for(int j=0;j<=nv;++j) {
		for(int i=0;i<nu;++i) {
			p[acc++].set(radius * cos(da * i), dv * j, -radius * sin(da * i));
		}
    }
    
    int itri = 0;
	for(int j=0;j<nv;++j) {
		bool isOdd = j & 1;
		
		int j1 = j + 1;
		
		for(int i=0;i<nu;++i) {
		
			int i1 = i + 1;
			if(i1 >= nu)
				i1 = 0;
		
			if(isOdd) 
				addOddCell(ind, itri, i, j, i1, j1, nu);
			else
				addEvenCell(ind, itri, i, j, i1, j1, nu);
			
			isOdd = !isOdd; 
		}
	}
	
	m_nu = nu;
	m_nv = nv;
	m_radius = radius;
    m_height = height;

    calculateVertexNormals();
}

/// j2 --- i1j2
///  |  \  |     even
/// j1 --- i1j1  
///  |  /  |     odd
/// i0 --- i1  

void YCylinder::addOddCell(unsigned* ind, int& tri,
				const int& i, const int& j,
				const int& i1, const int& j1,
				const int& nu)
{
	int tri3 = tri * 3;
	ind[tri3    ] = j * nu + i;
	ind[tri3 + 1] = j * nu + i1;
	ind[tri3 + 2] = j1 * nu + i1;
	tri++;
    
	tri3 = tri * 3;
	ind[tri3    ] = j * nu + i;
	ind[tri3 + 1] = j1 * nu + i1;
	ind[tri3 + 2] = j1 * nu + i;
	tri++;
}

void YCylinder::addEvenCell(unsigned* ind, int& tri,
				const int& i, const int& j,
				const int& i1, const int& j1,
				const int& nu)
{
	int tri3 = tri * 3;
	ind[tri3    ] = j * nu + i;
	ind[tri3 + 1] = j * nu + i1;
	ind[tri3 + 2] = j1 * nu + i;
	tri++;
    
	tri3 = tri * 3;
	ind[tri3    ] = j * nu + i1;
	ind[tri3 + 1] = j1 * nu + i1;
	ind[tri3 + 2] = j1 * nu + i;
	tri++;
}

}
