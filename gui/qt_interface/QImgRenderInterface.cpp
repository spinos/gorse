/*
 *  QImgRenderInterface.cpp
 *  aloe
 *
 *  Created by zhang on 18-2-22.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#include "QImgRenderInterface.h"
#include <interface/DisplayImage.h>

namespace alo {

QImgRenderInterface::QImgRenderInterface()
{}

QImage QImgRenderInterface::getQImage() const
{ return QImage(c_image()->bits(), 
	c_image()->xres(), c_image()->yres(), 
	QImage::Format_RGB32); 
}

}
