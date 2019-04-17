/*
 *  HSsdfIO.cpp
 *  aloe
 *
 *  Created by jian zhang on 3/10/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */

#include "HSsdfIO.h"

namespace alo {

HSsdfIO::HSsdfIO() 
{}

bool HSsdfIO::findSsdf(std::vector<std::string> & names)
{
	const int oldSize = names.size();
	HBase ga("/asset");
	ga.lsTypedChild<HSsdf>(names);
	ga.close();
	return names.size() > oldSize;
}

}