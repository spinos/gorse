/*
 *  HTriangleMesh.h
 *  testbcc
 *
 *  Created by jian zhang on 4/23/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once
#include <h5/HBase.h>

namespace alo {

class ATriangleMesh;

class HTriangleMesh : public HBase {
public:
	HTriangleMesh(const std::string & path);
	virtual ~HTriangleMesh();
	
	virtual char verifyType();
	virtual char save(ATriangleMesh * tri);
	virtual char load(ATriangleMesh * tri);
	
	char saveTriangleTexcoord(ATriangleMesh * tri);
	char loadTriangleTexcoord(ATriangleMesh * tri);
	
protected:
	char readAftCreation(ATriangleMesh * tri);
	
private:
	
};

}