/*
 *  UniformPlot2D.h
 *  aloe
 *
 *  Created by jian zhang on 9/14/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef ALO_UNIFORM_PLOT_2D_H
#define ALO_UNIFORM_PLOT_2D_H

#include <math/Array3.h>

namespace alo {

/// y = f(u,v)
/// m-by-n array per channel
/// up to 4 channels
/// column major
/// 0   m   ... (n-1)m
/// 1   m+1     (n-1)m+1
/// .   .       .
/// .   .       .
/// .   .       .
/// m-1 2m-1... nm-1

class UniformPlot2D {

public:
	enum FillMode {
		flFixed = 0,
		flVertical = 1,
		flHorizontal = 2
	};

private:
	Array3<float> m_data;
	float m_drScale;
	FillMode m_fmd;
	
public:
	UniformPlot2D();
	virtual ~UniformPlot2D();

/// M number of rows
/// N number of columns
/// P number of channels
	void create(const int & m, const int & n, const int & p = 1);
	void create(const Array3<float> & b);
	
	void setFillMode(FillMode x);
	void setDrawScale(float x);
	
	const int & numRows() const;
	const int & numCols() const;
	const int & numChannels() const;
	FillMode fillMode() const;
	const float & drawScale() const;
	
/// k-th channel
	float * y(const int & k=0);
	const float * y(const int & k=0) const;
	
	Array2<float> * channel(const int & k=0);
	const Array2<float> * channel(const int & k=0) const;
	
/// u column v row
	int iuv(const int & u, const int & v) const;
	
	const Array3<float> & data() const;
	
/// assuming [m,n] = size(x)
/// scale and shift to (0,1)
	void floatToColor(const float * x,
	                    const float scale = 1.f,
	                    const float shift = 0.f);
	
protected:

private:

};

}
#endif
