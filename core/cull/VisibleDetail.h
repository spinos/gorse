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
#include <math/Vector3F.h>

namespace alo {
class BaseCamera;
class VisibleDetail {

	SimpleBuffer<VisibilityState> m_visible;
	SimpleBuffer<LevelOfDetailSelect> m_detail;
	Vector3F m_viewPoint;
	Vector3F m_viewDirection;
	float m_deltaD;

public:
	VisibleDetail();
	virtual ~VisibleDetail();

	void create(int n);
	void setVisible(bool v);
	void setDetail(float x);
	void setDeltaDistance(float x);

	bool updateView(const BaseCamera &cam);

	VisibilityState *visibilities();
	LevelOfDetailSelect *levelOfDetails();
	const VisibilityState *c_visibilities() const;
	const LevelOfDetailSelect *c_levelOfDetails() const;

};

}

#endif
