/*
 *  ShapeMatchingConstraint.cpp
 *  aloe
 *  
 *  2019/10/6
 */
 
#include "ShapeMatchingConstraint.h"
#include <math/Constant.h>
#include <math/MatrixC33F.h>

namespace alo {

ShapeMatchingConstraint::ShapeMatchingConstraint() : 
_stiffness(1.0f)
{}

void ShapeMatchingConstraint::setOffset(const int x)
{ _offset = x; }

void ShapeMatchingConstraint::setNumVertices(const int x)
{ _numVertices = x; }

void ShapeMatchingConstraint::calculateMass(float *mass, const float *invMass, 
                    const int *ind)
{
    _totalMass = 0.f;
    const int end = _offset + _numVertices;
    for(int i=_offset;i<end;++i) {
        const int &vi = ind[i];
        const float mi = getMass(invMass[vi]);
        mass[i] = mi;
        _totalMass += mi;
    }
}

void ShapeMatchingConstraint::calculateP(Vector3F *p,
                        const float *mass,
                        const Vector3F *positions,
                        const int *ind)
{
    _centerOfMass.setZero();
    const int end = _offset + _numVertices;
    for(int i=_offset;i<end;++i) {
        const int &vi = ind[i];
        const Vector3F &pos = positions[vi];
        p[i] = pos;
        _centerOfMass += pos * mass[i]; 
    }
    _centerOfMass /= _totalMass;
    
    for(int i=_offset;i<end;++i) {
        p[i] -= _centerOfMass;
    }
}

void ShapeMatchingConstraint::setQ(Vector3F *q, const Vector3F *p,
                        const int *ind)
{
    const int end = _offset + _numVertices;
    for(int i=_offset;i<end;++i) {
        q[i] = p[i];
    }
}

void ShapeMatchingConstraint::calculateG(Vector3F *g,
                        const Vector3F *p, 
                        const Vector3F *q)
{
    MatrixC33F A_pq;
	A_pq.setZero();
    
    const int end = _offset + _numVertices;
    for(int i=_offset;i<end;++i) {

        const Vector3F &pv = p[i];
        const Vector3F &qv = q[i];
        
		A_pq.col(0)[0] += pv.x * qv.x;
		A_pq.col(0)[1] += pv.y * qv.x;
		A_pq.col(0)[2] += pv.z * qv.x;
		
		A_pq.col(1)[0] += pv.x * qv.y;
		A_pq.col(1)[1] += pv.y * qv.y;
		A_pq.col(1)[2] += pv.z * qv.y;
		
		A_pq.col(2)[0] += pv.x * qv.z;
		A_pq.col(2)[1] += pv.y * qv.z;
		A_pq.col(2)[2] += pv.z * qv.z;
		
	}
    
    A_pq.extractRotation(_rotq, 20);
	MatrixC33F R(_rotq);

    for(int i=_offset;i<end;++i) {
        
        const Vector3F &qv = q[i];
        
		g[i].x = R.col(0)[0] * qv.x
					+ R.col(1)[0] * qv.y
					+ R.col(2)[0] * qv.z
					+ _centerOfMass.x;
		g[i].y = R.col(0)[1] * qv.x
					+ R.col(1)[1] * qv.y
					+ R.col(2)[1] * qv.z
					+ _centerOfMass.y;
		g[i].z = R.col(0)[2] * qv.x
					+ R.col(1)[2] * qv.y
					+ R.col(2)[2] * qv.z
					+ _centerOfMass.z;
	}
    
}

const float &ShapeMatchingConstraint::stiffness() const
{ return _stiffness; }

float ShapeMatchingConstraint::getMass(const float &invMass)
{
    return invMass > Constant::VeryLow ? 1.f / invMass : Constant::VeryHigh;
}

void ShapeMatchingConstraint::solvePosition(Vector3F &pos, 
            const float* q_n1, const int &vi,
			const int& i, const Vector3F *g) const
{
    const int oi = _offset + i;
	pos.x = q_n1[vi * 3]     + (g[oi].x - q_n1[vi * 3]    ) * _stiffness;
	pos.y = q_n1[vi * 3 + 1] + (g[oi].y - q_n1[vi * 3 + 1]) * _stiffness;
	pos.z = q_n1[vi * 3 + 2] + (g[oi].z - q_n1[vi * 3 + 2]) * _stiffness;
}

}
