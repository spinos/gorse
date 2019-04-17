/*
 *  FHilbert.h
 *  aloe
 *
 *  Created by zhang on 18-2-17.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_SDS_F_HILBERT_H
#define ALO_SDS_F_HILBERT_H

#include <math/hilbert3.h>

namespace alo {

namespace sds {

struct FHilbert {
	
	float _u0[3];
	float _red[3];
	float _green[3];
	float _blue[3];
	int _level;
	int _range[2];
	
	FHilbert() : _level(10) 
	{}
	
	void setRange(const float* p, const float* q, 
					const int& level)
	{
		_range[0] = hilbert3DCoord(p[0], p[1], p[2],
						_u0[0], _u0[1], _u0[2],
						_red[0], _red[1], _red[2],
						_green[0], _green[1], _green[2],
						_blue[0], _blue[1], _blue[2],
						level);
		_range[1] = hilbert3DCoord(q[0], q[1], q[2],
						_u0[0], _u0[1], _u0[2],
						_red[0], _red[1], _red[2],
						_green[0], _green[1], _green[2],
						_blue[0], _blue[1], _blue[2],
						level);
	}

/// (center, half_span)
    void setCoord(const float& centerx,
					const float& centery,
					const float& centerz,
					const float& halfSpan)
    {
        _u0[0] = centerx;
		_u0[1] = centery;
		_u0[2] = centerz;
		_red[0] = halfSpan;
		_red[1] = 0;
		_red[2] = 0;
		_green[0] = 0;
		_green[1] = halfSpan;
		_green[2] = 0;
		_blue[0] = 0;
		_blue[1] = 0;
		_blue[2] = halfSpan;
    }
	
	void setOriginSpan(const float& originx,
					const float& originy,
					const float& originz,
					const float& span) 
	{
		const float spanH = span * .5f;
		_u0[0] = originx + spanH;
		_u0[1] = originy + spanH;
		_u0[2] = originz + spanH;
		_red[0] = spanH;
		_red[1] = 0;
		_red[2] = 0;
		_green[0] = 0;
		_green[1] = spanH;
		_green[2] = 0;
		_blue[0] = 0;
		_blue[1] = 0;
		_blue[2] = spanH;
	}
	
	int computeKey(const float* p) const
	{
		return hilbert3DCoord(p[0], p[1], p[2],
						_u0[0], _u0[1], _u0[2],
						_red[0], _red[1], _red[2],
						_green[0], _green[1], _green[2],
						_blue[0], _blue[1], _blue[2],
						_level);
	}
	
	int computeKey(const float* p, const int& level) const
	{
		return hilbert3DCoord(p[0], p[1], p[2],
						_u0[0], _u0[1], _u0[2],
						_red[0], _red[1], _red[2],
						_green[0], _green[1], _green[2],
						_blue[0], _blue[1], _blue[2],
						level);
	}
	
	void getBox(float* centerHalfSpan) const
	{
		centerHalfSpan[0] = _u0[0];
		centerHalfSpan[1] = _u0[1];
		centerHalfSpan[2] = _u0[2];
		centerHalfSpan[3] = _red[0];
	}
	
	void computeChildCoord(float* childCoord, const int& i,
				const float* parentCoord) const
	{
		float& h = childCoord[3];
		h = parentCoord[3] / 2.f;
		childCoord[0] = parentCoord[0] + h * HilbertSubNodeCoord[i][0];
		childCoord[1] = parentCoord[1] + h * HilbertSubNodeCoord[i][1];
		childCoord[2] = parentCoord[2] + h * HilbertSubNodeCoord[i][2];
	}
	
};

}

}

#endif
