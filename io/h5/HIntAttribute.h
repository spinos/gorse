/*
 *  HIntAttribute.h
 *  aloe
 *
 *
 */

#ifndef ALO_H_INT_ATTRIBUTE_H
#define ALO_H_INT_ATTRIBUTE_H

#include "HAttribute.h"

namespace alo {

class HIntAttribute : public HAttribute
{
public:
	HIntAttribute(const std::string & path);
	~HIntAttribute() {}
	
    virtual bool write(int *data);
	virtual bool read(int *data);
   
protected:
    virtual hid_t dataType() const;
	
};

}
#endif
