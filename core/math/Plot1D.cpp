/*
 *  Plot1D.cpp
 *  aloe
 *
 *  Created by jian zhang on 9/14/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */

#include "Plot1D.h"

namespace alo {

Plot1D::Plot1D() :
m_gt(GtLine),
m_lstyle(LsSolid),
m_mstyle(MsQuad)
{}

Plot1D::~Plot1D()
{}

void Plot1D::create(const int & n)
{ 
	m_xvar.create(n);
	m_yvar.create(n); 
	linspacex(n);
}

void Plot1D::create(const float * y, const int & n)
{ 
	m_xvar.create(n);
	m_yvar.create(y, n);
	linspacex(n);
}

void Plot1D::linspacex(const int & n)
{
	float h = 1.f / (n-1);
	for(int i=0;i<n;++i)
		m_xvar[i] = h * i;
}

void Plot1D::setColor(float r, float g, float b)
{ m_color[0] =r;
	m_color[1] =g;
	m_color[2] =b; }

const float* Plot1D::color() const
{ return m_color; }

float * Plot1D::x()
{ return m_xvar.v(); }

const float * Plot1D::x() const
{ return m_xvar.v(); }

const float * Plot1D::y() const
{ return m_yvar.v(); }

float * Plot1D::y()
{ return m_yvar.v(); }

const int & Plot1D::numY() const
{ return m_yvar.N(); }

void Plot1D::setLineStyle(Plot1D::LineStyle x)
{ m_lstyle = x; }

const Plot1D::GeomType & Plot1D::geomType() const
{ return m_gt; }

void Plot1D::setGeomType(GeomType t)
{ m_gt = t; }

void Plot1D::setMarkStyle(MarkStyle t)
{ m_mstyle = t; }

const Plot1D::MarkStyle & Plot1D::markStyle() const
{ return m_mstyle; }

const Plot1D::LineStyle & Plot1D::lineStyle() const
{ return m_lstyle; }

const VectorN<float> & Plot1D::data() const
{ return m_yvar; }

}