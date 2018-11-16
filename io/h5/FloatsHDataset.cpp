/*
 *  FloatsHDataset.cpp
 *  mallard
 *
 *  Created by jian zhang on 9/28/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include "FloatsHDataset.h"
namespace alo {

FloatsHDataset::FloatsHDataset(const std::string & path) : HDataset(path)
{
}

FloatsHDataset::~FloatsHDataset() {}

void FloatsHDataset::setNumFloats(int num)
{
	fDimension[0] = num;
	fDimension[1] = 0;
	fDimension[2] = 0;
}
	
int FloatsHDataset::numFloats() const
{
	return fDimension[0];
}

}