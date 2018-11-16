/*
 *  HStringAttribute.h
 *  aloe
 *
 *
 */
#ifndef ALO_H_STRING_ATTRIBUTE_H
#define ALO_H_STRING_ATTRIBUTE_H

#include "HAttribute.h"

namespace alo {

class HStringAttribute : public HAttribute
{
public:
	HStringAttribute(const std::string & path);
	~HStringAttribute();
	
	virtual bool write(const std::string & str);
	virtual bool read(std::string & str);
    
protected:
	virtual hid_t dataType() const;
    
};

/// variable length string attrib based on
/// http://www.hdfgroup.org/ftp/HDF5/examples/misc-examples/attrvstr.c
class HVLStringAttribute : public HStringAttribute
{
public:
	HVLStringAttribute(const std::string & path);
	~HVLStringAttribute();
	
	virtual bool write(const std::string &str);
    virtual bool read(std::string & str);
    
protected:
	virtual hid_t dataType() const;
    
};

}
#endif
