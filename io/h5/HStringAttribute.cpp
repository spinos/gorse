/*
 *  HStringAttribute.cpp
 *  aloe
 *
 *
 */

#include "HStringAttribute.h"
#include <iostream>
#ifdef LINUX
#include <stdlib.h>
#endif
namespace alo {

HStringAttribute::HStringAttribute(const std::string & path) : HAttribute(path) {}
HStringAttribute::~HStringAttribute() {}
	
hid_t HStringAttribute::dataType() const
{
	return H5T_NATIVE_CHAR;
}

bool HStringAttribute::write(const std::string & str)
{
	if(writeRaw(str.c_str()) < 0)
		return false;
	return true;
}

bool HStringAttribute::read(std::string & str)
{
	const int d = dataSpaceDimension();
	char * t = new char[d + 1];
	herr_t status = readRaw(t);
	if(status < 0) {
        delete[] t;
		return false;
    }
	t[d] = '\0';
	str = std::string(t);
	delete[] t;
	return true;
}

HVLStringAttribute::HVLStringAttribute(const std::string & path) : HStringAttribute(path) {}
HVLStringAttribute::~HVLStringAttribute() {}
	
hid_t HVLStringAttribute::dataType() const
{
	hid_t type = H5Tcopy (H5T_C_S1);
	H5Tset_size (type, H5T_VARIABLE);
	return type;
}

bool HVLStringAttribute::write(const std::string &str)
{
    const char *str_att[1];
    str_att[0] = str.c_str();
    herr_t status = H5Awrite(fObjectId, dataType(), &str_att);
    if(status < 0)
		return false;
	return true;
}

bool HVLStringAttribute::read(std::string & str)
{	
	hid_t ftype = H5Aget_type(fObjectId);
    H5T_class_t type_class = H5Tget_class (ftype);   

    if (type_class != H5T_STRING) {
		std::cout<<"\n type class is not H5T_STRING";
		return HStringAttribute::read(str);
	}
    
	char *t[1];
	herr_t status = H5Aread(fObjectId, dataType(), &t);
    if(status < 0)
		return false;
    
	str = std::string(t[0]);
	free( t[0]);
	return true;
}

}
