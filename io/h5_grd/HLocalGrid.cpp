/*
 *  HLocalGrid.cpp
 *
 *  2019/5/5
 */

#include "HLocalGrid.h"

namespace alo {

HLocalGrid::HLocalGrid(const std::string &name) : ver1::HBase(name)
{}

bool HLocalGrid::verifyType()
{
	return hasNamedAttr(".is_local_grid");
}

}
