/*
 *  GeodesicDistance.h
 *  aloe
 *
 */
 
#ifndef ALO_GEODESIC_DISTANCE_H
#define ALO_GEODESIC_DISTANCE_H

#include "MeshTopology.h"
#include "VertexEdgeConnection.h"
#include <math/SimpleBuffer.h>

namespace alo {

class GeodesicDistance : public MeshTopology {
   
    VertexEdgeConnection m_vertexEdgeConnection;
/// nabla f(x) per-face gradient of distance
/// directional rate of change of distance fucntion across each face
    SimpleBuffer<Vector3F> m_gradient;
   
public:
    GeodesicDistance();
    virtual ~GeodesicDistance();
    
    virtual void buildTopology(const ver1::ATriangleMesh *mesh) override;
    
    void calcEdgeLength(const ver1::ATriangleMesh *mesh);
/// O(n)
    int closestVertexTo(const Vector3F& ori, const ver1::ATriangleMesh *mesh) const;
    
    void calcDistanceFromVertex(int ori, const ver1::ATriangleMesh *mesh);
    void normalizeDistance(const ver1::ATriangleMesh *mesh);
    
    void calcGradient(const ver1::ATriangleMesh *mesh);
    
/// V <- V + x distance
    void deform(ver1::ATriangleMesh *mesh, const Vector3F &displacement) const;

protected:
    void initField();
    
private:
   void propagate(std::vector<int> &trials);
   static void CalcFaceGradient(Vector3F &grad, const float &d0, const float &d1, const float &d2,
                const Vector3F &p0, const Vector3F &p1, const Vector3F &p2);
   
};

}

#endif
