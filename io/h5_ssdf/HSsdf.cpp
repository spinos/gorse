/*
 *  HSsdf.cpp
 *  aloe
 *
 *  Created by jian zhang on 2/28/18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#include "HSsdf.h"

namespace alo {

HSsdf::HSsdf(const std::string & name) :
HBase(name)
{}

bool HSsdf::verifyType()
{
	if(!hasNamedAttr(".coord") ) return 0;
	if(!hasNamedAttr(".pql") ) return 0;
	if(!hasNamedData(".celli") ) return 0;
	if(!hasNamedData(".coarse") ) return 0;
	if(!hasNamedData(".fine") ) return 0;
	return 1;
}

}
