/*
 *  pointBox.h
 *
 *  2019/4/25
 */

#ifndef ALO_POINT_BOX_H
#define ALO_POINT_BOX_H

#include "miscfuncs.h"

namespace alo {

static const int AabbCornerInd[8][3] = {
{0, 1, 2},
{3, 1, 2},
{0, 4, 2},
{3, 4, 2},
{0, 1, 5},
{3, 1, 5},
{0, 4, 5},
{3, 4, 5}
};
	
/// a[3]
/// b[6] is (low, high)
/// i-th corner
inline void getAabbCorner(float *a, const float *b, int i)
{
	const int *ci = AabbCornerInd[i]; 
	a[0] = b[ci[0]];
	a[1] = b[ci[1]];
	a[2] = b[ci[2]];
}

/// a[3]
/// b[6] is (low, high)
inline void expandAabbByPoint(float *b, const float *a)
{
	if(b[0] > a[0]) b[0] = a[0];
	if(b[1] > a[1]) b[1] = a[1];
	if(b[2] > a[2]) b[2] = a[2];
	if(b[3] < a[0]) b[3] = a[0];
	if(b[4] < a[1]) b[4] = a[1];
	if(b[5] < a[2]) b[5] = a[2];
}

inline bool isPointOutsideBox(const float *a, const float *b)
{
    if(a[0] < b[0] || a[0] > b[3]) return true;
    if(a[1] < b[1] || a[1] > b[4]) return true;
    if(a[2] < b[2] || a[2] > b[5]) return true;
    return false;
}

inline float distanceOutsidePointToBox(const float *a, const float *b)
{
    float q[3];
    memcpy(q, a, 12);

    if(q[0] < b[0]) q[0] = b[0];
    if(q[0] > b[3]) q[0] = b[3];

    if(q[1] < b[1]) q[1] = b[1];
    if(q[1] > b[4]) q[1] = b[4];

    if(q[2] < b[2]) q[2] = b[2];
    if(q[2] > b[5]) q[2] = b[5];

    q[0] -= a[0];
    q[1] -= a[1];
    q[2] -= a[2];
    return sqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2]);
}

/// move a to box b
/// return distance moved
inline float movePointOntoBox(float *a, const float *b)
{
    float q[3];
    memcpy(q, a, 12);
    if(a[0] < b[0]) a[0] = b[0];
    if(a[0] > b[3]) a[0] = b[3];
    if(a[1] < b[1]) a[1] = b[1];
    if(a[1] > b[4]) a[1] = b[4];
    if(a[2] < b[2]) a[2] = b[2];
    if(a[2] > b[5]) a[2] = b[5];
    q[0] -= a[0];
    q[1] -= a[1];
    q[2] -= a[2];
    return sqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2]);
}

inline float distanceInsidePointToBox(const float *a, const float *b)
{
    float d = a[0] - b[0];
    if(d < 0) d = -d;
    float md = d;
    
    d = a[0] - b[3];
    if(d < 0) d = -d;
    
    if(md > d) md = d;
    
    d = a[1] - b[1];
    if(d < 0) d = -d;
    
    if(md > d) md = d;
    
    d = a[1] - b[4];
    if(d < 0) d = -d;
    
    if(md > d) md = d;
    
    d = a[2] - b[2];
    if(d < 0) d = -d;
    
    if(md > d) md = d;
    
    d = a[2] - b[5];
    if(d < 0) d = -d;
    
    if(md > d) md = d;
    
    return md;
}

inline int closestBoxSideToPoint(const float *a, const float *b)
{
    float md = 1e10f;
    int side = 0;

    for(int j=0;j<2;++j) {
        for(int i=0;i<3;++i) {
            float d = a[i] - b[j * 3 + i];
            if(d < 0) d = -d;

            if(d < 1e-3f) {
                return j * 3 + i;
            }

            if(md > d) {
                md = d;
                side = j * 3 + i;
            }
        }
    }
    return side;
}

static const float AabbSideNormal[6][3] = {
{-1.f, 0.f, 0.f},
{ 0.f,-1.f, 0.f},
{ 0.f, 0.f,-1.f},
{ 1.f, 0.f, 0.f},
{ 0.f, 1.f, 0.f},
{ 0.f, 0.f, 1.f}
};

/// point a on box b
inline void normalOnBox(float *nml, const float *a, const float *b)
{
    int side = closestBoxSideToPoint(a, b);

    memcpy(nml, AabbSideNormal[side], 12);
}

inline void randomPointInsideCube(float *q, const float *orih)
{
    q[0] = orih[0] + RandomF01() * orih[3];
    q[1] = orih[1] + RandomF01() * orih[3];
    q[2] = orih[2] + RandomF01() * orih[3];
}

static const int AabbSideODuDv[6][9] = {
{0,1,2, 0,1,5, 0,4,2}, /// -x
{3,1,5, 3,1,2, 3,4,5}, /// +x
{0,1,2, 3,1,2, 0,1,5}, /// -y
{0,4,5, 3,4,5, 0,4,2}, /// +y
{3,1,2, 0,1,2, 3,4,2}, /// -z
{0,1,5, 3,1,5, 0,4,5}  /// +z
};

inline void randomPointOnBoxSide(float *q, const float *b, int s)
{
    float odudv[9];
    for(int i=0;i<9;++i) odudv[i] = b[AabbSideODuDv[s][i]];

    odudv[3] -= odudv[0];
    odudv[4] -= odudv[1];
    odudv[5] -= odudv[2];
    odudv[6] -= odudv[0];
    odudv[7] -= odudv[1];
    odudv[8] -= odudv[2];

    const float u = RandomF01();
    const float v = RandomF01();
    q[0] = odudv[0] + odudv[3] * u + odudv[6] * v;
    q[1] = odudv[1] + odudv[4] * u + odudv[7] * v;
    q[2] = odudv[2] + odudv[5] * u + odudv[8] * v; 
}

}

#endif
