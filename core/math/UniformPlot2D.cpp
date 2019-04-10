/*
 *  UniformPlot2D.cpp
 *  aloe
 *
 *  Created by jian zhang on 9/14/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */

#include "UniformPlot2D.h"
#include <math/miscfuncs.h>

namespace alo {

UniformPlot2D::UniformPlot2D() :
m_fmd(flFixed),
m_drScale(1.f)
{}

UniformPlot2D::~UniformPlot2D()
{}

void UniformPlot2D::create(const int & m, const int & n, const int & p)
{ m_data.create(m,n,p); }

void UniformPlot2D::create(const Array3<float> & b)
{ m_data = b; }

void UniformPlot2D::setFillMode(FillMode x)
{ m_fmd = x; }

void UniformPlot2D::setDrawScale(float x)
{ m_drScale = x; }

const int & UniformPlot2D::numRows() const
{ return m_data.numRows(); }

const int & UniformPlot2D::numCols() const
{ return m_data.numCols(); }

const int & UniformPlot2D::numChannels() const
{ return m_data.numRanks(); }

UniformPlot2D::FillMode UniformPlot2D::fillMode() const
{ return m_fmd; }

const float & UniformPlot2D::drawScale() const
{ return m_drScale; }

float * UniformPlot2D::y(const int & k)
{ return channel(k)->v(); }

const float * UniformPlot2D::y(const int & k) const
{ return channel(k)->v(); }

int UniformPlot2D::iuv(const int & u, const int & v) const
{ return u * m_data.numRows() + v; }

Array2<float> * UniformPlot2D::channel(const int & k)
{ return m_data.rank(k); }

const Array2<float> * UniformPlot2D::channel(const int & k) const
{ return m_data.rank(k); }

const Array3<float> & UniformPlot2D::data() const
{ return m_data; }

static const float ColorCurve[6][3]= {
{ 0.f, 0.f, .65f},
{ 0.f, .35f, 1.f},
{.15f, 1.f, .95f},
{.95f, 1.f, .15f},
{1.f, .35f, 0.f},
{.65f, 0.f, 0.f}
};

static inline void plotColor(float & r, float & g, float & b,
    const float & x, 
    const float curve[6][3])
{
    float s;
    int i = x * 5.f;
    if(i>4) i=4;
    s = (x - .2f * i) * 5.f;
    
    r = curve[i][0] * (1.f - s) 
        +curve[i+1][0] * s;
    g = curve[i][1] * (1.f - s) 
        +curve[i+1][1] * s;
    b = curve[i][2] * (1.f - s) 
        +curve[i+1][2] * s;
}

void UniformPlot2D::floatToColor(const float * x,
	                    const float scale,
	                    const float shift)
{
    int n = numCols();
    int m = numRows();
	float * redv = y(0);
	float * greenv = y(1);
	float * bluev = y(2);
	float ssc;
		
    for(int j=0;j<n;++j) {
        for(int i=0;i<m;++i) {

            ssc = x[j*m+i] * scale + shift;
            Clamp01(ssc);
            plotColor(redv[j*m+i], greenv[j*m+i], bluev[j*m+i],
                    ssc, ColorCurve);
        }
    }
	
}

}