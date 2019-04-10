/*
 *  SampleWidget.h
 *  envt
 *
 *  Created by jian zhang on 9/14/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef SAMPLE_WIDGET_H
#define SAMPLE_WIDGET_H

#include <qt_lab/Plot1DWidget.h>
#include <interface/light.h>

namespace alo {
class SpaceTransform;
class Lambert;
class CookTorrance;
}

class SampleWidget : public alo::Plot1DWidget {

	Q_OBJECT
	
public:
	SampleWidget(QWidget *parent = 0);
	virtual ~SampleWidget();
	
	void generateSamples(const alo::EnvLightTyp* lit);
	
protected:

private:
	
	alo::SpaceTransform* m_tangentSpace;
	alo::Lambert* m_lambert;
	alo::CookTorrance* m_cook;
	
};

#endif
