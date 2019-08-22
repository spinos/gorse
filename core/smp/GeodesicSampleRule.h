/*
 *  GeodesicSampleRule.h
 *  gorse
 *
 *  2019/7/20 50 years since Apollo 11 Moon landing
 */

#ifndef ALO_GEODESIC_SAMPLE_RULE_H
#define ALO_GEODESIC_SAMPLE_RULE_H

#include <math/Matrix44F.h>

namespace alo {

template<typename T, typename Trng>
class GeodesicSampleRule {

	T m_pre;
	Trng *m_rng;
	int m_dataSize;
	int m_numSelected;
/// size of subset much smaller than size of data
	int m_maxNumSelected;
	float m_maxGeod;
    float m_minDistance;
    Matrix33F m_worldRotation;
    
public:

	GeodesicSampleRule();
	virtual ~GeodesicSampleRule();

	void setRng(Trng *x);
	void setMaxNumSelected(int x);
    void setMinDistance(float x);
    void setTransform(const Matrix44F &x);
	void calculateMaxGeod(const T *data, const int &n);
    void clear();
/// i-th sample in data
	bool accept(const T &x, const int &i);

	bool finished() const;
    void begin();
	
	Matrix33F getRotation(const T &s);

protected:

private:

};

template<typename T, typename Trng>
GeodesicSampleRule<T, Trng>::GeodesicSampleRule() :
m_rng(nullptr),
m_dataSize(0),
m_numSelected(0),
m_maxNumSelected(100),
m_maxGeod(0.f),
m_minDistance(1.f)
{}

template<typename T, typename Trng>
GeodesicSampleRule<T, Trng>::~GeodesicSampleRule()
{}

template<typename T, typename Trng>
void GeodesicSampleRule<T, Trng>::setRng(Trng *x)
{ m_rng = x; }

template<typename T, typename Trng>
void GeodesicSampleRule<T, Trng>::setMaxNumSelected(int x)
{ m_maxNumSelected = x; }

template<typename T, typename Trng>
void GeodesicSampleRule<T, Trng>::setMinDistance(float x)
{ m_minDistance = x; }

template<typename T, typename Trng>
void GeodesicSampleRule<T, Trng>::setTransform(const Matrix44F &x)
{ m_worldRotation = x.rotation(); }

template<typename T, typename Trng>
void GeodesicSampleRule<T, Trng>::calculateMaxGeod(const T *data, const int &n)
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

	m_dataSize = n;
}

template<typename T, typename Trng>
bool GeodesicSampleRule<T, Trng>::finished() const
{ return m_numSelected >= m_maxNumSelected; }

template<typename T, typename Trng>
bool GeodesicSampleRule<T, Trng>::accept(const T &x, const int &i)
{
/// same cell exclusion
	if(x._key == m_pre._key) return false;
    if(x._geod < m_maxGeod * .5f) return false;
	
	const float acceptRatio = (float)(m_maxNumSelected - m_numSelected) / (float)(m_dataSize - i) * 6.f;
	
	if(m_rng->randf1() > acceptRatio) return false;
    
    Vector3F worldNml = x._nml;
    m_worldRotation.transformInPlace(worldNml);
	
	if(worldNml.y < -.1f) return false;
	
/// normal-binormal check
	if(x._nml.dot(x._grad.normal()) > .3f)
		return false;
		
	if(m_numSelected > 0) {
		if(x._nml.dot(m_pre._nml) > .9f)
			return false;
		if(x._pos.distanceTo(m_pre._pos) < m_minDistance) 
			return false;
	}

	m_pre = x;
	m_numSelected++;
	return true;
}

template<typename T, typename Trng>
void GeodesicSampleRule<T, Trng>::begin()
{ 
    m_numSelected = 0; 
    m_pre._key = -1;
}

template<typename T, typename Trng>
Matrix33F GeodesicSampleRule<T, Trng>::getRotation(const T &s)
{
	Vector3F up = s._grad; up.normalize();
	Vector3F front = s._nml.cross(up); front.normalize();
	Vector3F side = up.cross(front); side.normalize();
	
	Matrix33F r;
	r.fill(side, up, front);
	
    float rollAngle = 6.28f * (m_rng->randf1() - .5f);

	Quaternion rq(rollAngle, up);
	Matrix33F roll(rq);
	r = r * roll;
    return r * m_worldRotation;
}

}

#endif
