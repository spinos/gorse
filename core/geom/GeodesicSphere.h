/*
 *  GeodesicSphere.h
 *  aloe
 *
 */

#ifndef ALO_GEODESIC_SPHERE_H
#define ALO_GEODESIC_SPHERE_H

#include "V1ATriangleMesh.h"

namespace alo {
    
class GeodesicSphere : public ver1::ATriangleMesh {
     
public:
    GeodesicSphere(int level = 5);
    virtual ~GeodesicSphere();
    
protected:

private:
    void subdivideTriangle(int level, unsigned & currentVertex, unsigned & currentIndex, 
        Vector3F * p, Int3 * idx, 
        const Vector3F & a, const Vector3F & b, const Vector3F & c);
    void subdivideUVTriangle(int level, int & faceVertexIndex, 
        Float2 * p, 
        const Float2 & a, const Float2 & b, const Float2 & c);
};

}
#endif
