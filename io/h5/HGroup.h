/*
 *  HGroup.h
 *  aloe
 *
 *  Created by jian zhang on 6/12/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef ALO_H_GROUP_H
#define ALO_H_GROUP_H

#include "HObject.h"

namespace alo {

class HGroup : public HObject {
public:
	HGroup(const std::string & path);
	virtual ~HGroup() {}
	
    bool isOpened() const;
	virtual char create();
	virtual char open();
	virtual void close();
	virtual int objectType() const;
};

}
#endif        //  #ifndef HGROUP_H
