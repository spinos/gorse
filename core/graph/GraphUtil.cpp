/*
 *  GraphUtil.cpp
 *  aloe
 *
 *  2019/9/25
 */
 
#include "GraphUtil.h"

namespace alo {
  
void GraphUtil::PushIndices(const std::vector<int> & a,
							std::vector<int> & b)
{
	std::vector<int>::const_iterator it = a.begin();
	for(;it!=a.end();++it) {
		b.push_back(*it);
    }
}
  
}
