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
/// hfov is tan(fov/2)
/// aspect ratio is h / w
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

/// behind all 6 faces
	bool isPointInside(const Vector3F &p) const;

	friend std::ostream& operator<<(std::ostream &output, const AFrustum & p);

/// all points of b inside frustum
	template<typename T>
	bool enclose(const T &b) const;
    
private:
	
};

template<typename T>
bool AFrustum::enclose(const T &b) const
{
	for(int i=0;i<b.numPoints();++i) {
		if(!isPointInside(b.X(i)))
			return false;
	}
	return true;
}

}

#endif
