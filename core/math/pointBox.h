/*
 *  pointBox.h
 *
 *  2019/4/25
 */

#ifndef ALO_POINT_BOX_H
#define ALO_POINT_BOX_H

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

static const float AabbSideNormal[6][3] = {
{-1.f, 0.f, 0.f},
{ 1.f, 0.f, 0.f},
{ 0.f,-1.f, 0.f},
{ 0.f, 1.f, 0.f},
{ 0.f, 0.f,-1.f},
{ 0.f, 0.f, 1.f}
};

inline void normalOnBox(float *nml, const float *a, const float *b)
{
    int side = 0;
    float md = a[0] - b[0];
    if(md < 0) md = -md;

    float d = a[0] - b[3];
    if(d < 0) d = -d;

    if(md > d) {
        md = d;
        side = 1;
    }

    d = a[1] - b[1];
    if(d < 0) d = -d;

    if(md > d) {
        md = d;
        side = 2;
    }

    d = a[1] - b[4];
    if(d < 0) d = -d;

    if(md > d) {
        md = d;
        side = 3;
    }

    d = a[2] - b[2];
    if(d < 0) d = -d;

    if(md > d) {
        md = d;
        side = 4;
    }

    d = a[2] - b[5];
    if(d < 0) d = -d;

    if(md > d) {
        md = d;
        side = 5;
    }

    memcpy(nml, AabbSideNormal[side], 12);
}

}

#endif
