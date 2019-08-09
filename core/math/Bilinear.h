/*
 * Bilinear.h
 * aloe
 *
 * c ----- d
 *     |
 *     y
 *     |
 * a --x-- b
 *
 * 2019/8/9
 */

#ifndef ALO_MATH_BI_LINEAR_H
#define ALO_MATH_BI_LINEAR_H

namespace alo {

template<typename T>
class Bilinear {

	T m_v[4];

public:

	Bilinear();
	~Bilinear();

	T *value();
	T interpolate(const float &x, const float &y) const;

protected:

private:
};

template<typename T>
Bilinear<T>::Bilinear()
{}

template<typename T>
Bilinear<T>::~Bilinear()
{}

template<typename T>
T *Bilinear<T>::value()
{ return m_v; }

template<typename T>
T Bilinear<T>::interpolate(const float &x, const float &y) const
{
	T a = m_v[0] * (1.f - x) + m_v[1] * x;
	T b = m_v[2] * (1.f - x) + m_v[3] * x;
	return a * (1.f - y) + b * y;
}

}

#endif
