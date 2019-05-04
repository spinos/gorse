/*
 *  HLocalGrid.cpp
 *
 *  2019/5/5
 */

#include "HLocalGrid.h"
#include <grd/IndexGrid.h>
#include <h5/HOocArray.h>

namespace alo {

HLocalGrid::HLocalGrid(const std::string &name) :
HBase(name)
{}

bool HLocalGrid::verifyType()
{
	return hasNamedAttr(".is_local_grid");
}

}
