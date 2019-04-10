/*
 *  ImgWidget.cpp
 *  envt
 *
 *  Created by jian zhang on 9/14/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */
#include <QtGui>
#include "ImgWidget.h"
#include <math/Array2.h>

using namespace alo;

ImgWidget::ImgWidget(QWidget *parent) : Plot2DWidget(parent)
{
	setMargin(2,2);
}

ImgWidget::~ImgWidget()
{}

void ImgWidget::addImage(const Array2<Vector3F>& img)
{
	const int& nu = img.numCols();
	const int& nv = img.numRows();
	
	UniformPlot2DImage* plot = new UniformPlot2DImage;
	plot->create(nv, nu, 3);
	
	for(int k=0;k<3;++k) {
	
		Array2<float>* dst = plot->channel(k);
		
		for(int i=0;i<nu;++i) {
			
			const Vector3F* src = img.column(i);
			float* udst = dst->column(i);
			
			for(int j=0;j<nv;++j) {
			
				const float* fsrc = (float* )&src[j];
				udst[j] = fsrc[k];
			}
		}
	}
	
	plot->updateImage();
	plot->setFillMode(UniformPlot2D::flHorizontal);
	
	Plot2DWidget::addImage(plot);
	
}
