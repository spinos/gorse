/*
 *  SampleState.h
 *  aloe img
 *
 *  uv coord weight access filter interpolate buffer
 *
 *  Created by jian zhang on 3/26/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef SAMPLE_STATE_H
#define SAMPLE_STATE_H

namespace alo {

class SampleState {

	float m_p[2];
	int m_u[2];
	int m_v[2];
	float m_wei[2];
/// [box_corners][channels]
	float m_buf[4][4];
	float m_exposureWeight;
	float m_levelOfDetail;
	int m_rank;
	
public:

	enum AccessMode {
		Clamp = 0,
		Wrap = 1
	};
	
	enum FilterMode {
		Nofilter = 0,
		Nearest = 1,
		Box = 2
	};
	
	enum ChannelMode {
		Red = 0,
		Green = 1,
		Blue = 2,
		Alpha = 3,
		RGB,
		RGBA,
		Luminance
	};
	
	SampleState();
	~SampleState();
	
	void setAccess(AccessMode u, AccessMode v);
	void setFilter(FilterMode x);
	void setChannel(ChannelMode x);
	void setExposure(const float& x);
	void setLevelOfDetail(const float& x);
	
	FilterMode filter() const;
	ChannelMode channel() const;
	AccessMode accessU() const;
    AccessMode accessV() const;

	float* p();
	int* u();
	int* v();
	float* buf(int corner);
	float& levelOfDetail();
/// direction in cartesian to spherical p
/// normalize p to [0,1]
	void toLongitudeLatitude(const float* direction);
	void computeCoord(const int& nu, const int& nv);
	void computeValue(float* dst, int rank);
	void setValue(float* dst, int rank) const;
	void setValue(float* dst) const;
	void computeValue(float* dst);
	
private:

	AccessMode m_accessU;
	AccessMode m_accessV;
	FilterMode m_filter;
	ChannelMode m_channel;
	
};
	
}

#endif
