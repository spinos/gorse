/*
 *  PixelProjector.cpp
 *  aloe
 *
 *  Created by jian zhang on 2/5/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#include "PixelProjector.h"
#include "BufferBlock.h"
#include "ImageFragment.h"
#include <math/miscfuncs.h>
#include <iostream>
namespace alo {

PixelProjector::PixelProjector()
{}

PixelProjector::~PixelProjector()
{}

float PixelProjector::reproject(BufferBlock& blk, const ImageFragment& y_t) const
{
	const int& t = blk.age();
	//const float alpha = 1.f - 1.f / (1.f + t);
	const float diva = blk.inverseAge();
	float residual = 0.f, den = 0.f;
	
	ImageFragment* h_tm1 = blk.fragment();
/// same size
	const int n = h_tm1->fragmentSize();
	
	float* redH = h_tm1->colorComponent(0); 
	const float* redY = y_t.colorComponent(0); 
	for(int i=0;i<n;++i) {
		Accumulate(redH[i], redY[i], diva, residual, den);
	}
	
	float* greenH = h_tm1->colorComponent(1); 
	const float* greenY = y_t.colorComponent(1); 
	for(int i=0;i<n;++i) {
		Accumulate(greenH[i], greenY[i], diva, residual, den);
	}
	
	float* blueH = h_tm1->colorComponent(2); 
	const float* blueY = y_t.colorComponent(2); 
	for(int i=0;i<n;++i) {
		Accumulate(blueH[i], blueY[i], diva, residual, den);
	}
	
	float* alphaH = h_tm1->colorComponent(3); 
	const float* alphaY = y_t.colorComponent(3); 
	for(int i=0;i<n;++i) {
		Accumulate(alphaH[i], alphaY[i], diva, residual, den);
	}
	
    residual /= 3.f + t;
    
    return residual;
}

void PixelProjector::Overwrite(float& h_t, const float& y_t, float& residual, float& den)
{ 
	float err = h_t - y_t;
	err= Absolute<float>(err);
	if(err > .5e-3f) {
	    residual += err;
	    den += 1.f;
	}
	
	h_t = y_t;
}

void PixelProjector::Blend(float& h_t, const float& y_t, const float& a, 
								float& residual, float& den)
{
    float err = h_t;
	
	h_t *= a;
	h_t += (1.f - a) * y_t; 
	
	err -= h_t;
	err = Absolute<float>(err);
	if(err > .5e-3f) {
	    residual += err;
	    den += 1.f;
	}
}

void PixelProjector::Accumulate(float& h_t, const float& y_t, const float& a,
	                    float& residual, float& den)
{
    if(a > .6f)
        return Overwrite(h_t, y_t, residual, den);

    h_t += y_t;
    
    float err = h_t * a - y_t;
    err = Absolute<float>(err);
    if(err > .5e-3f) {
	    residual += err;
	    den += 1.f;
	}
}

}
