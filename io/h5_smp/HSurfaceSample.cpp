/*
 *  HSurfaceSample.cpp
 *  gorse
 *
 *  2019/8/12
 */

#include "HSurfaceSample.h"

namespace alo {

HSurfaceSample::HSurfaceSample(const std::string &name) :
ver1::HBase(name)
{}

bool HSurfaceSample::verifyType()
{
	return hasNamedAttr(".is_surf_sample");
}

}
