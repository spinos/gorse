/*
 *  GeodesicSampleRule.h
 *  gorse
 *
 *  2019/8/27
 */

#ifndef ALO_GEODESIC_SAMPLE_RULE_H
#define ALO_GEODESIC_SAMPLE_RULE_H

#include <math/Matrix44F.h>
#include "SpatialSampleRule.h"

namespace alo {

template<typename Trng, typename Tc>
class GeodesicSampleRule : public SpatialSampleRule<Trng, Tc> {

	float m_maxGeod;
	float m_minGrowDistance;
    Matrix33F m_worldRotation;
    
public:

	GeodesicSampleRule(Tc *sfc);
	virtual ~GeodesicSampleRule();

    void setTransform(const Matrix44F &x);
	void setMinGrowDistance(const float &x);
	template<typename Ts>
	void calculateMaxGeod(const Ts *data, const int &n);

/// i-th sample in data
	template<typename Ts>
	bool accept(const Ts &x, const int &i);
	
	template<typename Ts>
	Matrix33F getRotation(const Ts &s);

protected:

private:

};

template<typename Trng, typename Tc>
GeodesicSampleRule<Trng,  Tc>::GeodesicSampleRule(Tc *sfc) : SpatialSampleRule<Trng, Tc>(sfc),
m_maxGeod(0.f),
m_minGrowDistance(.5f)
{}

template<typename Trng, typename Tc>
GeodesicSampleRule<Trng, Tc>::~GeodesicSampleRule()
{}

template<typename Trng, typename Tc>
void GeodesicSampleRule<Trng,  Tc>::setTransform(const Matrix44F &x)
{ m_worldRotation = x.rotation(); }

template<typename Trng, typename Tc>
void GeodesicSampleRule<Trng,  Tc>::setMinGrowDistance(const float &x)
{ m_minGrowDistance = x; }

template<typename Trng, typename Tc>
template<typename Ts>
void GeodesicSampleRule<Trng,  Tc>::calculateMaxGeod(const Ts *data, const int &n)
{
	m_maxGeod = 0;
	int preK = -1;
	int nCell = 0;
	for(int i=0;i<n;++i) {
		if(preK < data[i]._key) {
			preK = data[i]._key;
			nCell++;
		}
		
		if(m_maxGeod < data[i]._geod)
			m_maxGeod = data[i]._geod;
	}
	//std::cout << "\n n cell " << nCell
	//			<< "\n max geodesic distance " << m_maxGeod;

}

template<typename Trng, typename Tc>
template<typename Ts>
bool GeodesicSampleRule<Trng,  Tc>::accept(const Ts &x, const int &i)
{
    if(x._geod < m_maxGeod * m_minGrowDistance) return false;
	
	if(rng()->randf1() > getAcceptRation(i, 4.f) ) return false;
    
    Vector3F worldNml = x._nml;
    m_worldRotation.transformInPlace(worldNml);
	
	if(worldNml.y < 0.f) return false;
	
/// normal-binormal check
	if(x._nml.dot(x._grad.normal()) > .5f)
		return false;
		
	if(SpatialSampleRule<Trng, Tc>::rejectByPosition(x._pos)) 
		return false;

	SpatialSampleRule<Trng, Tc>::select(x._pos);
	return true;
}

template<typename Trng, typename Tc>
template<typename Ts>
Matrix33F GeodesicSampleRule<Trng,  Tc>::getRotation(const Ts &s)
{
/// gradient as x
	Vector3F side = s._grad; side.normalize();
	Vector3F front = side.cross(s._nml); front.normalize();
	Vector3F up = front.cross(side); side.normalize();
	
	Matrix33F r;
	r.fill(side, up, front);
	
    float rollAngle = rng()->randf1() * .13f;
	if(rng()->binary()) rollAngle = -rollAngle;
	
	Quaternion rq(rollAngle, up);
	Matrix33F roll(rq);
	r = r * roll;
    return r * m_worldRotation;
}

}

#endif
