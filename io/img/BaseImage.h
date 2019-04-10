/*
 *  BaseImage.h
 *  aloe
 *
 *  Created by jian zhang on 9/1/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
 
#ifndef BASE_IMAGE_H
#define BASE_IMAGE_H

#include "SampleState.h"
#include <string>

namespace alo {

template<typename T>
struct Array3;

class BaseImage {

		bool m_isValid;
    	int m_imageWidth, m_imageHeight;
		std::string m_fileName;
		
public:
	enum IFormat {
		FUnknown = 0,
		FExr = 1
	};
	
	enum ChannelRank {
		None = 0,
		RED = 1,
		RGB = 3,
		RGBA = 4,
		RGBAZ = 5
	};
	
	BaseImage();
	virtual ~BaseImage();
	
	bool read(const std::string & filename);
	
	const bool & isValid() const;
	const std::string & fileName() const;
	
	virtual IFormat formatName() const;
	std::string formatNameStr() const;
	std::string channelRankStr() const;
	
	void setChannelRank(ChannelRank x);
	ChannelRank channelRank() const;
    void setImageWidth(int x);
    void setImageHeight(int x);
	const int& imageWidth() const;
	const int& imageHeight() const;
/// h / w
	const float aspectRatio() const;
	int pixelLoc(float s, float t, bool flipV, int pixelRank) const;
	
	bool isRGBAZ() const;
	void verbose() const;
/// 2 to the power of ? closest to image size
	void getResampleSize(int & xdim, int & ydim) const;
/// scale longer dimension to refSize
	void getThumbnailSize(int & xdim, int & ydim,
					const int & refSize = 256) const;
/// longer dimension no lower than lowSize after divided by 2 for ? times
	int getMaxCompressLevel(int lowSize = 128) const;
	
protected:
	virtual bool readImage(const std::string & filename);
	
private:
    ChannelRank m_channelRank;
	
};

}

#endif
