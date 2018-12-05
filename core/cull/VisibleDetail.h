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
#include "VisibilityState.h"
#include "LevelOfDetailSelect.h"

namespace alo {

class VisibleDetail {

	SimpleBuffer<VisibilityState> m_visible;
	SimpleBuffer<LevelOfDetailSelect> m_detail;

public:
	VisibleDetail();
	virtual ~VisibleDetail();

	void create(int n);
	void setVisible(bool v);
	void setDetail(float x);

	VisibilityState *visibilities();
	LevelOfDetailSelect *levelOfDetails();
	const VisibilityState *c_visibilities() const;
	const LevelOfDetailSelect *c_levelOfDetails() const;

};

}

#endif
