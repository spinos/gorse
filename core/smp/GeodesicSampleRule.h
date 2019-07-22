/*
 *  GeodesicSampleRule.h
 *  gorse
 *
 *  2019/7/20 50 years since Apollo 11 Moon landing
 */

#ifndef ALO_GEODESIC_SAMPLE_RULE_H
#define ALO_GEODESIC_SAMPLE_RULE_H

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

public:

	GeodesicSampleRule();
	virtual ~GeodesicSampleRule();

	void setRng(Trng *x);
	void setMaxNumSelected(int x);
/// per cell
/// max_n_sel / n_cell
/// after set max_n_sel
	void setAcceptRatio(const T *data, const int &n);
/// i-th sample in data
	bool accept(const T &x, const int &i);

	bool finished() const;
	
	void getPitchAndRoll(float &pitch, float &rollAngle, const T &x);

protected:

private:

};

template<typename T, typename Trng>
GeodesicSampleRule<T, Trng>::GeodesicSampleRule() :
m_rng(nullptr),
m_dataSize(0),
m_numSelected(0),
m_maxNumSelected(100),
m_maxGeod(0.f)
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
void GeodesicSampleRule<T, Trng>::setAcceptRatio(const T *data, const int &n)
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
	std::cout << "\n n cell " << nCell
				<< "\n max geodesic distance " << m_maxGeod;

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
	
	const float acceptRatio = (float)(m_maxNumSelected - m_numSelected) / (float)(m_dataSize - i) * 6.f;
	
	if(m_rng->randf1() > acceptRatio) return false;
	
	if(x._geod < m_maxGeod * .33f)
		return false;
	
	if(x._nml.y < -.1f)
		return false;
	
	if(x._grad.y < -.1f)
		return false;
	
/// normal-binormal check
	if(x._nml.dot(x._grad.normal()) > .3f)
		return false;
		
	if(m_numSelected > 0) {
		if(x._nml.dot(m_pre._nml) > .9f)
			return false;
		if(x._pos.distanceTo(m_pre._pos) < 2.f) 
			return false;
		if(Absolute<float>(x._geod - m_pre._geod) <  2.f)
			return false;
	}

	m_pre = x;
	m_numSelected++;
	return true;
}

template<typename T, typename Trng>
void GeodesicSampleRule<T, Trng>::getPitchAndRoll(float &pitch, float &rollAngle, const T &x)
{
	float alpha = 1.f - x._geod / m_maxGeod;
	pitch = m_rng->randf(0.1f * alpha, 0.7f * alpha, 65535);
	rollAngle = 3.14f * (m_rng->randf1() - .5f);
}

}

#endif
