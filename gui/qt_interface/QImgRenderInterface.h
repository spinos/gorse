/*
 *  QImgRenderInterface.h
 *  aloe
 *
 *  render interface output to qimage
 *
 *  Created by zhang on 18-2-22.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */
 
#ifndef ALO_Q_IMG_RENDER_INTERFACE_H
#define ALO_Q_IMG_RENDER_INTERFACE_H

#include <QImage>

#include <interface/RenderInterface.h>

namespace alo {

class QImgRenderInterface : public RenderInterface {

public:

    QImgRenderInterface();

    QImage getQImage() const;
	
protected:

private:

};

}

#endif

