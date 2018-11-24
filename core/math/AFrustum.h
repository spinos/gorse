/*
 *  AFrustum.h
 *  aloe
 *
 *
 */

#ifndef ALO_A_FRUSTUM_H
#define ALO_A_FRUSTUM_H

#include "Vector3F.h"

namespace alo {

class Matrix44F;

class AFrustum {

///	origin at lower-left corner
///
///  3 - 2 near  7 - 6 far
///  |   |       |   |
///  0 - 1       4 - 5
	Vector3F m_v[8];
	
public:
	AFrustum();
/// clip is negative in camera space
	void set(const float & hfov,
			const float & aspectRatio,
			const float & clipNear,
			const float & clipFar,
			const Matrix44F & mat);
	void setOrtho(const float & hwith,
			const float & aspectRatio,
			const float & clipNear,
			const float & clipFar,
			const Matrix44F & mat);
	
/// origin at left-up corner, center position, right and down deviation of first pixel, 
/// at near and far clip
/// 
///   - 1 near   - 4 far
/// | 0        | 3
/// 2          5
///
    void toRayFrame(Vector3F * dst, const int & gridX, const int & gridY) const;

    const Vector3F &X(int idx) const;
	Vector3F supportPoint(const Vector3F & v, Vector3F * localP = 0) const;

	friend std::ostream& operator<<(std::ostream &output, const AFrustum & p);
    
private:
	
};

}

#endif
