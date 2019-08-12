/*
 *  Hermite.h
 *  gorse
 *
 *  pt_0 ---> tg_0
 *
 *
 *                  pt_1 ---> tg_1
 *
 *  0-------x-------1
 *
 *  2019/8/12
 */

#ifndef ALO_MATH_HERMITE_H
#define ALO_MATH_HERMITE_H

namespace alo {
	
template<typename T>
class Hermite {

public:
	
	static T interpolate(const float x, const T *pt, const T *tg);
	
};

template<typename T>
T Hermite<T>::interpolate(const float x, const T *pt, const T *tg)
{
	float s2 = x * x;
	float s3 = s2 * x;
	float h1 =  2.f * s3 - 3.f * s2 + 1.f;          // calculate basis function 1
	float h2 = -2.f * s3 + 3.f * s2;              // calculate basis function 2
	float h3 =   s3 - 2.f * s2 + x;         // calculate basis function 3
	float h4 =   s3 -  s2;              // calculate basis function 4
	return (pt[0] * h1 +                    // multiply and sum all funtions
             pt[1] * h2 +                    // together to build the interpolated
             tg[0] * h3 +                    // point along the curve.
             tg[1] * h4);
}

}

#endif
