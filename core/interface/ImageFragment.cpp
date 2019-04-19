/*
 *  ImageFragment.cpp
 *  aloe
 *
 *  Created by jian zhang on 2/5/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#include "ImageFragment.h"
#include <math/miscfuncs.h>
#include <iostream>

namespace alo {

ImageFragment::ImageFragment() :
m_fragWidth(BUFFER_BLOCK_TILE_SIZE),
m_fragHeight(BUFFER_BLOCK_TILE_SIZE)
{ memset(m_color, 0, MAX_BUFFER_BLOCK_SIZE<<4);}

void ImageFragment::setColor(const float* x, const int& i)
{
	m_color[0][i] = x[0];
	m_color[1][i] = x[1];
	m_color[2][i] = x[2];
	m_color[3][i] = x[3];
}

const int& ImageFragment::fragmentWidth() const
{ return m_fragWidth; }

const int& ImageFragment::fragmentHeight() const
{ return m_fragHeight; }

int ImageFragment::fragmentSize() const
{ return m_fragWidth * m_fragHeight; }

float* ImageFragment::colorComponent(int i)
{ return m_color[i]; }

const float* ImageFragment::colorComponent(int i) const
{ return m_color[i]; }

}
