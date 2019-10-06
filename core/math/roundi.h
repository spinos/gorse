/*
 *  roundi.h
 *  aloe
 *
 *  round up to multiplication of i
 *
 */

#ifndef ALO_ROUND_I_H
#define ALO_ROUND_I_H

namespace alo {
    
inline int Round16(int a)
{
    if(a < 16) return 16;
    int b = a>>4;
    if(a & 15) b++;
    return b<<4;
}

inline int Round32(int a)
{
    if(a < 32) return 32;
    int b = a>>5;
    if(a & 31) b++;
    return b<<5;
}

inline int Round64(int a)
{
    if(a < 64) return 64;
    int b = a>>6;
    if(a & 63) b++;
    return b<<6;
}

inline int Round128(int a)
{
    if(a < 128) return 128;
    int b = a>>7;
    if(a & 127) b++;
    return b<<7;
}

inline int Round256(int a)
{
    if(a < 256) return 256;
    int b = a>>8;
    if(a & 255) b++;
    return b<<8;
}

inline int Round1024(int a)
{
    if(a < 1024) return 1024;
    int b = a>>10;
    if(a & 1023) b++;
    return b<<10;
}

inline int Round2048(int a)
{
    if(a < 2048) return 2048;
    int b = a>>11;
    if(a & 2047) b++;
    return b<<11;
}

inline int Round4096(int a)
{
    if(a < 4096) return 4096;
    int b = a>>12;
    if(a & 4095) b++;
    return b<<12;
}

}

#endif

