/*
 *  HIntAttribute.cpp
 *  aloe
 *
 *
 */

#include "HIntAttribute.h"

namespace alo {

HIntAttribute::HIntAttribute(const std::string & path) : HAttribute(path)
{
}

hid_t HIntAttribute::dataType() const
{
	return H5T_NATIVE_INT;
}

bool HIntAttribute::write(int *data)
{
	if(writeRaw((char *)data) < 0)
		return false;
	return true;
}

bool HIntAttribute::read(int *data)
{
	if(readRaw((char *)data) < 0)
		return false;
	return true;
}

}