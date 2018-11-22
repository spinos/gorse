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

inline int Round32(int a)
{
    int b = a>>5;
    if(a & 31) b++;
    return b<<5;
}

inline int Round256(int a)
{
    int b = a>>8;
    if(a & 255) b++;
    return b<<8;
}

inline int Round1024(int a)
{
    int b = a>>10;
    if(a & 1023) b++;
    return b<<10;
}

inline int Round2048(int a)
{
    int b = a>>11;
    if(a & 2047) b++;
    return b<<11;
}

inline int Round4096(int a)
{
    int b = a>>12;
    if(a & 4095) b++;
    return b<<12;
}

}

#endif

