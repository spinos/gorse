/*
 *  VisibleDetail.h
 *  aloe
 *
 *  SoA (visible, level-of-detail) 
 *
 */

#ifndef ALO_VISIBLE_DETAIL_H
#define ALO_VISIBLE_DETAIL_H

#include <math/SimpleBuffer.h>

namespace alo {

class VisibleDetail {

	SimpleBuffer<bool> m_visible;
	SimpleBuffer<float> m_detail;

public:
	VisibleDetail();
	virtual ~VisibleDetail();

	void create(int n);
	void set(bool v, float d);

	bool *visibilities();
	float *levelOfDetails();
	const bool *c_visibilities() const;
	const float *c_levelOfDetails() const;

};

}

#endif
