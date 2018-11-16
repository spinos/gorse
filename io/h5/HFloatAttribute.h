/*
 *  HFloatAttribute.h
 *  aloe
 *
 *
 */

#ifndef ALO_H_FLOAT_ATTRIBUTE_H
#define ALO_H_FLOAT_ATTRIBUTE_H

#include "HAttribute.h"

namespace alo {

class HFloatAttribute : public HAttribute
{
public:
	HFloatAttribute(const std::string & path);
	~HFloatAttribute() {}
    
	virtual bool write(float *data);
	virtual bool read(float *data);
    
protected:
	virtual hid_t dataType() const;
    
};

}

#endif
