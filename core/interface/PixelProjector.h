/*
 *  PixelProjector.h
 *  aloe
 *  
 *  project rendered fragment onto block fragment
 *  by exponential convergence factor alpha that controls
 *  the blending between history and current input colors
 *  h(t) <- alpha h(t-1) + (1 - alpha)y(t)
 *  alpha is some function of t, i.e., 1 - 1/(1+t)
 *
 *  Created by jian zhang on 2/5/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef PIXEL_PROJECTOR_H
#define PIXEL_PROJECTOR_H

namespace alo {

class ImageFragment;

class PixelProjector {

public:

	PixelProjector();
	virtual ~PixelProjector();
	
	virtual float reproject(ImageFragment* h_tm1, const int t, const float diva, const ImageFragment& y_t) const;
	
protected:

private:
	static void Overwrite(float& h_t, const float& y_t, 
	                    float& residual, float& den);
	static void Blend(float& h_t, const float& y_t, const float& a, 
	                    float& residual, float& den);
	static void Accumulate(float& h_t, const float& y_t, const float& a,
	                    float& residual, float& den);
	
};

}

#endif
