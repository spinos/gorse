/*
 *  GridSamples.h
 *  gorse
 *
 *  2019/5/5
 */

#ifndef ALO_GRD_GRID_SAMPLES_H
#define ALO_GRD_GRID_SAMPLES_H

#include <sds/SpaceFillingVector.h>
#include <math/Vector3F.h>

namespace alo {

namespace grd {

template<typename T>
class GridSamples {

	sds::SpaceFillingVector<T> m_samples;

public:

	typedef T SampleTyp;

	GridSamples();

	template<typename T1>
	void create(const T1 *obj);

	const sds::SpaceFillingVector<T> &samples() const;

};

template<typename T>
GridSamples<T>::GridSamples()
{}

template<typename T>
template<typename T1>
void GridSamples<T>::create(const T1 *obj)
{
	m_samples.clear();
	//obj-> template genSamples<T>(m_samples);
    obj->genSamples(m_samples);
}

template<typename T>
const sds::SpaceFillingVector<T> &GridSamples<T>::samples() const
{ return m_samples; }

struct PointSample {
	Vector3F _pos;
	int _key;
};

typedef GridSamples<PointSample> PointGridSamplesTyp;

}

}

#endif
