#ifndef LINE_MATH_H
#define LINE_MATH_H

#include "Vector3F.h"
#include "miscfuncs.h"

namespace alo {

/// http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html
inline float distancePointLine(const Vector3F & P0, 
                        const Vector3F & P1, const Vector3F & P2)
{
    //return sqrt(((P0 - P1).cross(P0 - P2)).length2() / (P2 - P1).length2()); 
    return sqrt(((P2 - P1).cross(P1 - P0)).length2() / (P2 - P1).length2()); 
}

inline bool distancePointLineSegment(float & d,
						const Vector3F & P0, 
                        const Vector3F & P1, const Vector3F & P2)
{
	Vector3F v12 = P2 - P1;
	Vector3F v10 = P0 - P1;

	if(v10.dot(v12) < 0.f ) {
		d = v10.length();
		return true;
	}
		
	Vector3F v20 = P0 - P2;
	
	if(v20.dot(v12) > 0.f ) {
		d = v20.length();
		return true;
	}
	
    d = sqrt((v10.cross(v20)).length2() / v12.length2() );
	return true;
}

///      p0
///      |
///      d
///      |
///  p1->q-------p2
///
///      p0
///      |
///      d
///      |
///      q<------p1-----------p2
inline void projectPointLineSegment(Vector3F & q, float &t,
						const float & d,
						const Vector3F & P0, 
                        const Vector3F & P1, const Vector3F & P2,
                        const Vector3F &vp1p2)
{
	Vector3F v10 = P0 - P1;
	t = sqrt(v10.length2() - d * d);
	if(v10.dot(vp1p2) < 0) t = -t;
	q = P1 + vp1p2 * t;
}

/// find q on line (p1, p2) closest to p0
/// t is distance from p1
inline void projectPointLine(Vector3F & q, float &t,
						const Vector3F & P0, 
                        const Vector3F & P1, const Vector3F & P2,
                        const Vector3F &vp1p2)
{
    projectPointLineSegment(q, t, distancePointLine(P0, P1, P2), P0, P1, P2, vp1p2);
}

// http://mathworld.wolfram.com/SkewLines.html
inline bool areIntersectedOrParallelLines(const Vector3F & P1, const Vector3F & P2,
                        const Vector3F & P3, const Vector3F & P4)
{
    return (P3 - P1).dot((P2 - P1).cross(P4 - P3)) == 0.f;
}

inline bool arePerpendicularLines(const Vector3F & P1, const Vector3F & P2,
                        const Vector3F & P3, const Vector3F & P4)
{
    return CloseToZero((P2 - P1).dot(P4 - P3));
}

inline bool areParallelLines(const Vector3F & P1, const Vector3F & P2,
                        const Vector3F & P3, const Vector3F & P4)
{
    return ((P2 - P1).cross(P4 - P3)).length2() < 1e-4f;
}

inline bool areIntersectedLines(const Vector3F & P1, const Vector3F & P2,
                        const Vector3F & P3, const Vector3F & P4)
{
    if(areIntersectedOrParallelLines(P1, P2, P3, P4))
        return (!areParallelLines(P1, P2, P3, P4));
    return false;
}

// http://mathworld.wolfram.com/Line-LineDistance.html
inline float distanceBetweenSkewLines(const Vector3F & P1, const Vector3F & P2,
                        const Vector3F & P3, const Vector3F & P4)
{
    const Vector3F n = (P2 - P1).cross(P4 - P3);
    return Absolute((P3 - P1).dot(n)) / n.length();
}

inline float distanceBetweenLines(const Vector3F & P1, const Vector3F & P2,
                        const Vector3F & P3, const Vector3F & P4)
{
    if(areIntersectedOrParallelLines(P1, P2, P3, P4)) {
        if(areParallelLines(P1, P2, P3, P4))
            return distancePointLine(P1, P3, P4);
        return 0.f;
    }
    return distanceBetweenSkewLines(P1, P2, P3, P4);
}

}
#endif        //  #ifndef LINE_MATH_H

