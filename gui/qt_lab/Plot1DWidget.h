/*
 *  Plot1DWidget.h
 *  aloe
 *  
 *  plot y = f(x)
 *  Created by jian zhang on 9/9/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_PLOT_1D_WIDGET_H
#define ALO_PLOT_1D_WIDGET_H

#include <qt/BaseImageWidget.h>
#include <math/Vector2F.h>

namespace alo {

class Plot1D;

class Plot1DWidget : public BaseImageWidget {

	Q_OBJECT
	
public:
	Plot1DWidget(QWidget *parent = 0);
	virtual ~Plot1DWidget();
	
/// horizontal and vertical bound 
/// low high number of intervals
	void setBound(const float & hlow, const float & hhigh, 
					const int & hnintv,
					const float & vlow, const float & vhigh, 
					const int & vnintv);
					
	void addVectorPlot(Plot1D * p);
	
public slots:
   	
protected:
	virtual void clientDraw(QPainter * pr);
	
/// left-up
	QPoint luCorner() const;
/// right-bottom
	QPoint rbCorner() const;
/// from draw coordinate
	Vector2F toRealSpace(const int & x,
	                    const int & y) const;
/// remap input [0,1] to bound
	float xToBound(const float & x) const;
	
private:
	void drawCoordsys(QPainter * pr) const;
	void drawHorizontalInterval(QPainter * pr) const;
	void drawVerticalInterval(QPainter * pr) const;
	void drawPlot(const Plot1D * plt, QPainter * pr);
	void drawLine(const Plot1D * plt, QPainter * pr);
	void drawMark(const Plot1D * plt, QPainter * pr);
	void drawColumn(const Plot1D * plt, QPainter * pr);

private:
	Vector2F m_hBound;
	Vector2F m_vBound;
	int m_niterval[2];
	
	std::vector<Plot1D *> m_curves;
	
};

}
#endif
