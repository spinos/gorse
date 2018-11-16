/*
 *  HAttribute.h
 *  aloe
 *
 *  Created by jian zhang on 12/21/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef ALO_H_ATTRIBUTE_H
#define ALO_H_ATTRIBUTE_H

#include "HObject.h"

namespace alo {

class HAttribute : public HObject {
public:
	HAttribute(const std::string & path);
	virtual ~HAttribute() {}
	
	virtual char create(int dim, hid_t parentId = FileIO.fFileId);

	virtual char open(hid_t parentId = FileIO.fFileId);
	virtual void close();
	virtual int objectType() const;
	virtual hid_t dataType();
	
	int dataSpaceDimension() const;
};

}
#endif
