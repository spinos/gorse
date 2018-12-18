/*
 *  VisibleDetail.h
 *  aloe
 *
 *  SoA (visible, level-of-detail) 
 *  track view direction, eye postion, screen width changes
 *
 */

#ifndef ALO_VISIBLE_DETAIL_H
#define ALO_VISIBLE_DETAIL_H

#include <math/SimpleBuffer.h>
#include "VisibilityState.h"
#include "LevelOfDetailSelect.h"

namespace alo {
class PerspectiveCamera;
class VisibleDetail {

	SimpleBuffer<VisibilityState> m_visible;
	SimpleBuffer<LevelOfDetailSelect> m_detail;
	LevelOfDetailParam m_param;
	float m_deltaD;

public:
	VisibleDetail();
	virtual ~VisibleDetail();

	void create(int n);
	void setVisible(bool v);
	void setDetail(float x);
	void setDetail(int x, int i);
	void setDeltaDistance(float x);

	bool updateView(const PerspectiveCamera &cam);

	VisibilityState *visibilities();
	LevelOfDetailSelect *levelOfDetails();
	const VisibilityState *c_visibilities() const;
	const LevelOfDetailSelect *c_levelOfDetails() const;
	const LevelOfDetailParam &param() const;

};

}

#endif
