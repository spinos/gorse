/*
 *  HIntAttribute.h
 *  aloe
 *
 *  Created by jian zhang on 12/21/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_H_INT_ATTRIBUTE_H
#define ALO_H_INT_ATTRIBUTE_H

#include "HAttribute.h"

namespace alo {

class HIntAttribute : public HAttribute
{
public:
	HIntAttribute(const std::string & path);
	~HIntAttribute() {}
	
	virtual char write(int *data);
	virtual char read(int *data);
};

}
#endif
