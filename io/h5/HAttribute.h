/*
 *  HAttribute.h
 *  aloe
 *
 *  small 1d array of fixed length except variable length string 
 *
 */
 
#ifndef ALO_H_ATTRIBUTE_H
#define ALO_H_ATTRIBUTE_H

#include "HObject.h"

namespace alo {

class HAttribute : public HObject {
public:
	HAttribute(const std::string & path);
	virtual ~HAttribute() {}
	
	virtual char create(int dim, hid_t parentId = FileIO.fFileId);
	virtual char open(hid_t parentId = FileIO.fFileId);
	virtual void close();
    
protected:
	virtual int objectType() const;
	virtual hid_t dataType() const = 0;
	
	int dataSpaceDimension() const;
	herr_t writeRaw(const char *data);
	herr_t readRaw(char *data);

};

}
#endif
