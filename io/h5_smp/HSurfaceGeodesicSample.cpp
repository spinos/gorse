/*
 *  HSurfaceGeodesicSample.cpp
 *  gorse
 *
 *  2019/7/15
 */

#include "HSurfaceGeodesicSample.h"

namespace alo {

HSurfaceGeodesicSample::HSurfaceGeodesicSample(const std::string &name) :
ver1::HBase(name)
{}

bool HSurfaceGeodesicSample::verifyType()
{
	return hasNamedAttr(".is_surf_geod_sample");
}

}
