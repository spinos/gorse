#ifndef ALO_RAY_PLANE_H
#define ALO_RAY_PLANE_H

namespace alo {

/// find t along ray defined by p0, n0
/// intersects plane defined by p1, n1
/// n0, n1 is mornalized

inline bool rayPlaneIntersectDoubleSided(float &t,
						const Vector3F & p0, const Vector3F &n0,
                        const Vector3F & p1, const Vector3F &n1)
{
    float below = n0.dot(n1);
	float d = -p1.dot(n1);
	t = -(p0.dot(n1) + d) / below;
	return t>0;
}

/// p0 is on the front facing side of the plane
inline bool rayPlaneIntersect(float &t,
						const Vector3F & p0, const Vector3F &n0,
                        const Vector3F & p1, const Vector3F &n1)
{
    const float below = n0.dot(n1);
    if(below > -1e-4f) return false;
	const float d = -p1.dot(n1);
	t = -(p0.dot(n1) + d) / below;
	return t>0;
}

inline Vector3F projectPointOnPlane(const Vector3F & p0,
                        const Vector3F & p1, const Vector3F &n1)
{
	const float d = -p1.dot(n1);
	float t = p0.dot(n1) + d;
	return p0 - n1 * t;
}

}

#endif
