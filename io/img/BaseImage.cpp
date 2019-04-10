/*
 *  BaseImage.cpp
 *  aloe
 *
 *  Created by jian zhang on 9/1/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "BaseImage.h"
#include <iostream>
namespace alo {

BaseImage::BaseImage() : 
m_isValid(false),
m_fileName("")
{}

BaseImage::~BaseImage()
{}

bool BaseImage::read(const std::string & filename)
{ 
	m_isValid = readImage(filename); 
	m_fileName = "";
	if(m_isValid) m_fileName = filename;
	
	return m_isValid;
}

const bool & BaseImage::isValid() const
{ return m_isValid; }

const std::string & BaseImage::fileName() const
{ return m_fileName; }

bool BaseImage::readImage(const std::string & filename)
{ return false; }

BaseImage::IFormat BaseImage::formatName() const
{ return FUnknown; }

const int& BaseImage::imageWidth() const
{ return m_imageWidth; }
	
const int& BaseImage::imageHeight() const
{ return m_imageHeight; }

void BaseImage::setImageWidth(int x)
{ m_imageWidth = x; }

void BaseImage::setImageHeight(int x)
{ m_imageHeight = x; }

const float BaseImage::aspectRatio() const
{ return (float)m_imageHeight/(float)m_imageWidth; }

int BaseImage::pixelLoc(float s, float t, bool flipV, int pixelRank) const
{
	if(flipV) t = 1.f - t;
	int x = m_imageWidth * s;
	int y = m_imageHeight * t;
	if(x < 0) x = 0;
	if(y < 0) y = 0;
	if(x > m_imageWidth - 1) x = m_imageWidth - 1;
	if(y > m_imageHeight - 1) y = m_imageHeight - 1;
	return (y * m_imageWidth + x) * pixelRank;
}

bool BaseImage::isRGBAZ() const 
{ return m_channelRank == RGBAZ; }

void BaseImage::setChannelRank(ChannelRank x)
{ m_channelRank = x; }
	
BaseImage::ChannelRank BaseImage::channelRank() const
{ return m_channelRank; }

std::string BaseImage::formatNameStr() const
{
	if(formatName() < 1) return "unknown";
	return "exr";
}

std::string BaseImage::channelRankStr() const
{
	if(m_channelRank == RGB)
		return "RGB";
	if(m_channelRank == RGBA)
		return "RGBA";
	return "RGBAZ";
}

void BaseImage::verbose() const
{
	if(!isValid() ) {
		std::cout<<"\n invalid image file ";
		return;
	}
		
	std::cout<<"\n image_file: "<<fileName()
			<<"\n format: "<<formatNameStr()
			<<"\n size: "<<imageWidth()<<" x "<<imageHeight()
			<<"\n channel: "<<channelRankStr()<<"\n";
	
}

void BaseImage::getResampleSize(int & xdim, int & ydim) const
{
	const int& w = imageWidth();
	xdim = 16;
	for(;;) {
		int x2 = xdim<<1;
		if(x2 >= w ) {
			if(x2 - w < w - xdim) {
				xdim = x2;
			} 
			break;
		}
		xdim = x2;
	}
	const int& h = imageHeight();
	ydim = 16;
	for(;;) {
		int y2 = ydim<<1;
		if(y2 >= h ) {
			if(y2 - h < h - ydim) {
				ydim = y2;
			} 
			break;
		}
		ydim = y2;
	}
}

int BaseImage::getMaxCompressLevel(int lowSize) const
{
	int x = imageHeight();
	if(x > imageWidth() ) {
		x = imageWidth();
	}
	
	int l = 0;
	for(;;) {
		int x2 = x>>1;
		if(x2 < lowSize) {
			break;
		}
		x = x2;
		l++;
	}
	return l;
}

void BaseImage::getThumbnailSize(int & xdim, int & ydim,
					const int & refSize) const
{
	const float r = aspectRatio();
	if(r < 1.f) {
		xdim = refSize;
		ydim = (float)refSize * r;
	} else {
		xdim = (float)refSize / r;
		ydim = refSize;
	}
}

}
