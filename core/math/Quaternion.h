/*
 *  Quaternion,h
 *  aloe
 *
 *  (w, x, y, z)
 *
 *  rotation by angle a around the u axis
 *
 */

#ifndef ALO_QUATERNION_H
#define ALO_QUATERNION_H

#include "Vector3F.h"

namespace alo {

class Quaternion {
public:
    Quaternion();
    Quaternion(float d, float a, float b, float c);
    Quaternion(const float & angle, const Vector3F & axis);
	void set(const float & angle, const Vector3F & axis);
	void set(float d, float a, float b, float c);
	const float magnitude() const;
	void normalize();
	void inverse();
	
	Quaternion operator*( const Quaternion & b ) const;	
	Quaternion progress(const Vector3F & angularVelocity, const float & timeStep) const;
	
	static void Slerp(Quaternion& qOut, 
					const Quaternion& qA,
					const Quaternion& qB,
					const float& t);
					
	Quaternion conjugate() const;
					
	float w, x, y, z;
};

}
#endif        //  #ifndef QUATERNION_H

