/*
 *  z-order curve with max order 9
 *  aloe
 *
 */

#ifndef ALO_MORTON_3_H
#define ALO_MORTON_3_H

#include "miscfuncs.h"

namespace alo {

// http://stackoverflow.com/questions/18529057/produce-interleaving-bit-patterns-morton-keys-for-32-bit-64-bit-and-128bit
inline int expandBits(int v) 
{ 
    // v = (v * 0x00010001u) & 0xFF0000FFu; 
    // v = (v * 0x00000101u) & 0x0F00F00Fu; 
    // v = (v * 0x00000011u) & 0xC30C30C3u; 
    // v = (v * 0x00000005u) & 0x49249249u; 
    
    v = (v | v << 16) & 0xFF0000FFu; 
    v = (v | v << 8) & 0x0F00F00Fu; 
    v = (v | v << 4) & 0xC30C30C3u; 
    v = (v | v << 2) & 0x49249249u; 
    return v; 
}

// Calculates a 30-bit Morton code for the 
// given 3D point located within the unit cube [0,1].
inline int encodeMorton3D(int x, int y, int z) 
{ 
    Clamp_0_1023<int>(x); 
    Clamp_0_1023<int>(y); 
    Clamp_0_1023<int>(z); 
    int xx = expandBits(x); 
    int yy = expandBits(y); 
    int zz = expandBits(z); 
    return xx << 2 | yy << 1 | zz; 
}

// decoding morton code to cartesian coordinate
// https://fgiesen.wordpress.com/2009/12/13/decoding-morton-codes/

inline unsigned Compact1By2(unsigned x)
{
  x &= 0x09249249;                  // x = ---- 9--8 --7- -6-- 5--4 --3- -2-- 1--0
  x = (x ^ (x >>  2)) & 0x030c30c3; // x = ---- --98 ---- 76-- --54 ---- 32-- --10
  x = (x ^ (x >>  4)) & 0x0300f00f; // x = ---- --98 ---- ---- 7654 ---- ---- 3210
  x = (x ^ (x >>  8)) & 0xff0000ff; // x = ---- --98 ---- ---- ---- ---- 7654 3210
  x = (x ^ (x >> 16)) & 0x000003ff; // x = ---- ---- ---- ---- ---- --98 7654 3210
  return x;
}

inline void decodeMorton3D(unsigned code, unsigned & x, unsigned & y, unsigned & z)
{
    x = Compact1By2(code >> 2);
    y = Compact1By2(code >> 1);
    z = Compact1By2(code >> 0);
}

inline void decodeMortoni(int* u, int code)
{
    u[0] = Compact1By2(code >> 2);
    u[1] = Compact1By2(code >> 1);
    u[2] = Compact1By2(code >> 0);
	if(u[0]==1023) u[0] = 1024;
	if(u[1]==1023) u[1] = 1024;
	if(u[2]==1023) u[2] = 1024;
}

}

#endif        //  #ifndef APH_MORTON_3_H

