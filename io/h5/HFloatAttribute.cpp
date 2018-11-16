/*
 *  HFloatAttribute.cpp
 *  aloe
 *
 *
 */

#include "HFloatAttribute.h"

namespace alo {

HFloatAttribute::HFloatAttribute(const std::string & path) : HAttribute(path)
{}

hid_t HFloatAttribute::dataType() const
{
	return H5T_NATIVE_FLOAT;
}

bool HFloatAttribute::write(float *data)
{
	if(writeRaw((char *)data) < 0)
		return false;
	return true;
}

bool HFloatAttribute::read(float *data)
{
	if(readRaw((char *)data) < 0)
		return false;
	return true;
}

}