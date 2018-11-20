#ifndef ALO_ROUND_I_H
#define ALO_ROUND_I_H

namespace alo {

inline int Round32(int a)
{
    int b = a>>5;
    if(a & 31) b++;
    return b<<5;
}

}

#endif

