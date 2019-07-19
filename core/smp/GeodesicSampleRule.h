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
	int m_numSelected;
/// size of subset much smaller than size of data
	int m_maxNumSelected;
	float m_cellRatio;

public:

	GeodesicSampleRule();
	virtual ~GeodesicSampleRule();

	void setRng(Trng *x);
	void setMaxNumSelected(int x);
/// per cell
/// max_n_sel / n_cell
/// after set max_n_sel
	void setAcceptRatio(const T *data, const int &n);

	bool accept(const T &x);

	bool finished() const;

protected:

private:

};

template<typename T, typename Trng>
GeodesicSampleRule<T, Trng>::GeodesicSampleRule() :
m_rng(nullptr),
m_numSelected(0),
m_maxNumSelected(100),
m_cellRatio(.5f)
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
	int preK = -1;
	int nCell = 0;
	for(int i=0;i<n;++i) {
		if(preK < data[i]._key) {
			preK = data[i]._key;
			nCell++;
		}
	}

	int samplesPerCell = n / nCell;

	m_cellRatio = (float)m_maxNumSelected / (float)nCell;

}

template<typename T, typename Trng>
bool GeodesicSampleRule<T, Trng>::finished() const
{ return m_numSelected >= m_maxNumSelected; }

template<typename T, typename Trng>
bool GeodesicSampleRule<T, Trng>::accept(const T &x)
{ 
	if(m_numSelected > 0) {


	}

	m_pre = x;
	return true;
}

}

#endif
