
/*
 *  AFrustum.cpp
 *  aloe
 *
 *
 */

#include "AFrustum.h"
#include "Matrix44F.h"
#include <iostream>

namespace alo {

AFrustum::AFrustum() {}

void AFrustum::set(const float & hfov,
			const float & aspectRatio,
			const float & clipNear,
			const float & clipFar,
			const Matrix44F & mat)
{
	float nearRight = -clipNear * hfov;
	float nearLeft = -nearRight;
	float nearUp = nearRight * aspectRatio;
	float nearBottom = -nearUp;
	float farRight = -clipFar * hfov;
	float farLeft = -farRight;
	float farUp = farRight * aspectRatio;
	float farBottom = -farUp;
	
	m_v[0] = mat.transform(Vector3F(nearLeft, nearBottom, clipNear));
	m_v[1] = mat.transform(Vector3F(nearRight, nearBottom, clipNear));
	m_v[2] = mat.transform(Vector3F(nearRight, nearUp, clipNear));
	m_v[3] = mat.transform(Vector3F(nearLeft, nearUp, clipNear));
	m_v[4] = mat.transform(Vector3F(farLeft, farBottom, clipFar));
	m_v[5] = mat.transform(Vector3F(farRight, farBottom, clipFar));
	m_v[6] = mat.transform(Vector3F(farRight, farUp, clipFar));
	m_v[7] = mat.transform(Vector3F(farLeft, farUp, clipFar));
}

void AFrustum::setOrtho(const float & hwith,
			const float & aspectRatio,
			const float & clipNear,
			const float & clipFar,
			const Matrix44F & mat)
{
	float hUp = hwith * aspectRatio;
	
	m_v[0] = mat.transform(Vector3F(-hwith, -hUp, clipNear));
	m_v[1] = mat.transform(Vector3F(hwith, -hUp, clipNear));
	m_v[2] = mat.transform(Vector3F(hwith, hUp, clipNear));
	m_v[3] = mat.transform(Vector3F(-hwith, hUp, clipNear));
	m_v[4] = mat.transform(Vector3F(-hwith, -hUp, clipFar));
	m_v[5] = mat.transform(Vector3F(hwith, -hUp, clipFar));
	m_v[6] = mat.transform(Vector3F(hwith, hUp, clipFar));
	m_v[7] = mat.transform(Vector3F(-hwith, hUp, clipFar));
}

void AFrustum::toRayFrame(Vector3F * dst, const int & gridX, const int & gridY) const
{
    dst[1] = (m_v[2] - m_v[3]) / gridX;
    dst[2] = (m_v[0] - m_v[3]) / gridY;
	dst[0] = m_v[3] + dst[1] * .5f + dst[2] * .5f;
    
    dst[4] = (m_v[6] - m_v[7]) / gridX;
    dst[5] = (m_v[4] - m_v[7]) / gridY;
	dst[3] = m_v[7] + dst[4] * .5f + dst[5] * .5f;
}

const Vector3F &AFrustum::X(int idx) const
{ return m_v[idx]; }

Vector3F AFrustum::supportPoint(const Vector3F & v, Vector3F * localP) const
{
	float maxdotv = -1e8f;
    float dotv;
	
    Vector3F res, q;
    for(int i=0; i < 8; i++) {
        q = m_v[i];
        dotv = q.dot(v);
        if(dotv > maxdotv) {
            maxdotv = dotv;
            res = q;
            if(localP) *localP = q;
        }
    }
    
    return res;
}

std::ostream& operator<<(std::ostream &output, const AFrustum & p)
{
	output <<" near [" << p.X(0) << p.X(1) << p.X(2) << p.X(3) << "]"
			<< "\n far [" << p.X(4) << p.X(5) << p.X(6) << p.X(7) << "]";
    return output;
}

}
