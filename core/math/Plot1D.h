/*
 *  Plot1D.h
 *  aloe
 *
 *  Created by jian zhang on 9/14/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef ALO_PLOT_1D_H
#define ALO_PLOT_1D_H

#include <math/VectorN.h>

namespace alo {

/// y = f(x)
/// n-points
class Plot1D {

public:
	enum GeomType {
		GtLine = 0,
		GtMark = 1,
		GtColumn,
	};
	
	enum LineStyle {
		LsSolid = 0,
		LsDash = 1,
		LsDot = 2
	};
	
	enum MarkStyle {
		MsQuad = 0,
		MsTriangle = 1,
		MsDiamond = 2
	};
	
private:
	VectorN<float> m_yvar;
	VectorN<float> m_xvar;
	float m_color[3];
	GeomType m_gt;
	LineStyle m_lstyle;	
	MarkStyle m_mstyle;
	
public:

	Plot1D();
	virtual ~Plot1D();
	
	void create(const int & n);
	void create(const float * y, const int & n);
	void setColor(float r, float g, float b);
	const float* color() const;
	const float * x() const;
	float * x();
	const float * y() const;
	float * y();
	const int & numY() const;
	void setLineStyle(LineStyle x);
	const LineStyle & lineStyle() const;
	const GeomType & geomType() const;
	const VectorN<float> & data() const;
	void setGeomType(GeomType t);
	void setMarkStyle(MarkStyle t);
	const MarkStyle & markStyle() const;
	
private:
	void linspacex(const int & n);
	
};

}
#endif
