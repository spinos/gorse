/*
 *  Absolute.h
 *  aloe
 *
 *  2019/10/6
 */
 
#ifndef ALO_MATH_ABSOLUTE_H
#define ALO_MATH_ABSOLUTE_H
 
namespace alo {

template<typename T>
class Absolute {
public:
    static T f(T const& a)
    {
        return (a >= 0.0) ? a : -a;
    }
};

}

#endif
