/*
 *  RandomProfileRule.h
 *  gorse
 *
 *  random tubular profile
 *
 *  2019/8/11
 */

#ifndef ALO_RANDOM_PROFILE_RULE_H
#define ALO_RANDOM_PROFILE_RULE_H

#include <math/Matrix33F.h>
#include <math/Float2.h>

namespace alo {

namespace tbl {

template <typename T>
class RandomProfileRule
{
	T *m_rng;
	Vector3F m_v0;
	Vector3F m_vm;

public:

	RandomProfileRule(T *rng);
	virtual ~RandomProfileRule();

	void setFirstDirection(const Vector3F &q);
	void setMajorDirection(const Vector3F &q);

	const Vector3F &firstDirection() const;

	Vector3F genDirection(const Float2 &pitchYaw, const float &mean,
						const float &size);

protected:

private:
};

template <typename T>
RandomProfileRule<T>::RandomProfileRule(T *rng) :
m_v0(0.f, 1.f, 0.f),
m_vm(0.f, 1.f, 0.f)
{ m_rng = rng; }

template <typename T>
RandomProfileRule<T>::~RandomProfileRule()
{}

template <typename T>
void RandomProfileRule<T>::setFirstDirection(const Vector3F &q)
{ m_v0 = q; }

template <typename T>
void RandomProfileRule<T>::setMajorDirection(const Vector3F &q)
{ m_vm = q; }

template <typename T>
const Vector3F &RandomProfileRule<T>::firstDirection() const
{ return m_v0; }

template <typename T>
Vector3F RandomProfileRule<T>::genDirection(const Float2 &pitchYaw, const float &mean,
						const float &size)
{
	float a = pitchYaw.x * (m_rng->randf1() - .5f);
	float b = pitchYaw.y * (m_rng->randf1() - .5f);
	float c = mean + (m_rng->randf1() - .5f) * size;

	Vector3F r = m_vm;

	Quaternion q1(a, Vector3F::YAxis);
	Matrix33F m1(q1);
	Quaternion q2(b, m1.transform(Vector3F::ZAxis));

	m1 *= Matrix33F(q2);
	m1.transformInPlace(r);

	return r * c;
}

}

}

#endif
