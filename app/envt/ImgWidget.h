/*
 *  ImgWidget.h
 *  envt
 *
 *  Created by jian zhang on 9/14/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef IMG_WIDGET_H
#define IMG_WIDGET_H

#include <qt_lab/Plot2DWidget.h>
#include <math/Vector3F.h>
#include <math/Array2.h>

class ImgWidget : public alo::Plot2DWidget {

	Q_OBJECT
	
public:
	ImgWidget(QWidget *parent = 0);
	virtual ~ImgWidget();
	
	void addImage(const alo::Array2<alo::Vector3F>& img);
	
protected:

private:
	
};

#endif
