/*
 *  Hexahedron.cpp
 *  aloe
 *
 */

#include "Hexahedron.h"
#include "BoundingBox.h"

namespace alo {

Hexahedron::Hexahedron()
{ memset(m_p, 0, 96); }

void Hexahedron::set(const BoundingBox &aabb)
{
	m_p[0] = aabb.getMin();
	m_p[1].set(aabb.getMax(0), aabb.getMin(1), aabb.getMin(2));
	m_p[2].set(aabb.getMin(0), aabb.getMax(1), aabb.getMin(2));
	m_p[3].set(aabb.getMax(0), aabb.getMax(1), aabb.getMin(2));
	m_p[4].set(aabb.getMin(0), aabb.getMin(1), aabb.getMax(2));
	m_p[5].set(aabb.getMax(0), aabb.getMin(1), aabb.getMax(2));
	m_p[6].set(aabb.getMin(0), aabb.getMax(1), aabb.getMax(2));
	m_p[7] = aabb.getMax();
}

Vector3F Hexahedron::center() const
{ return (m_p[0] + m_p[7]) * .5f; }

float Hexahedron::size() const
{ return m_p[0].distanceTo(m_p[7]); }

Vector3F Hexahedron::supportPoint(const Vector3F & v, Vector3F * localP) const
{
	float maxdotv = -1e8f;
	
    Vector3F res;
    for(int i=0; i < 8; i++) {
        const Vector3F &q = m_p[i];
        float dotv = q.dot(v);
        if(dotv > maxdotv) {
            maxdotv = dotv;
            res = q;
            if(localP) *localP = q;
        }
    }
    
    return res;
}

int Hexahedron::numPoints() const
{ return 8; }

const Vector3F &Hexahedron::X(int i) const
{ return m_p[i]; }

}
