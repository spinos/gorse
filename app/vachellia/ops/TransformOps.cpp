/*
 *  TransformOps.cpp
 *  vachellia
 *
 */

#include "TransformOps.h"

namespace alo {

TransformOps::TransformOps()
{}

TransformOps::~TransformOps()
{}

bool TransformOps::hasMenu() const
{ return true; }

void TransformOps::getMenuItems(std::vector<std::pair<std::string, int > > &ks) const 
{
    ks.push_back(std::make_pair("Focus", 0));
}

void TransformOps::recvAction(int x) 
{
    if(x == 0) {}
}

}
