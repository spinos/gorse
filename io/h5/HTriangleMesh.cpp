/*
 *  HTriangleMesh.cpp
 *  testbcc
 *
 *  Created by jian zhang on 4/23/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#include "HTriangleMesh.h"
#include <vector>
#include <sstream>
#include <iostream>
#include <foundation/BaseBuffer.h>
#include <geom/ATriangleMesh.h>

namespace alo {

HTriangleMesh::HTriangleMesh(const std::string & path) : HBase(path) 
{
}

HTriangleMesh::~HTriangleMesh() {}

char HTriangleMesh::verifyType()
{
	if(!hasNamedAttr(".ntri"))
		return 0;

	if(!hasNamedAttr(".nv"))
		return 0;
	
	return 1;
}

char HTriangleMesh::save(ATriangleMesh * tri)
{
	int nv = tri->numPoints();
	if(!hasNamedAttr(".nv"))
		addIntAttr(".nv");
	
	writeIntAttr(".nv", &nv);
	
	int nt = tri->numTriangles();
	if(!hasNamedAttr(".ntri"))
		addIntAttr(".ntri");
	
	writeIntAttr(".ntri", &nt);
	
	if(!hasNamedData(".p"))
	    addVector3Data(".p", nv);
	
	writeVector3Data(".p", nv, (Vector3F *)tri->points());
	
	if(!hasNamedData(".v"))
	    addIntData(".v", nt * 3);
	
	writeIntData(".v", nt * 3, (int *)tri->indices());

	return 1;
}

char HTriangleMesh::saveTriangleTexcoord(ATriangleMesh * tri)
{
	int nt = tri->numTriangles();
	if(!hasNamedData(".tritexcoord")) {
	    addFloatData(".tritexcoord", nt * 6);
	}
	
	writeFloatData(".tritexcoord", nt * 6, (float*)tri->c_triangleTexcoords() );
	
	return 1;
}

char HTriangleMesh::load(ATriangleMesh * tri)
{
	int nv = 3;
	
	readIntAttr(".nv", &nv);
	
	int nt = 1;
	
	readIntAttr(".ntri", &nt);
	
	tri->create(nv, nt);
	
	return readAftCreation(tri);
}

char HTriangleMesh::readAftCreation(ATriangleMesh * tri)
{
	readVector3Data(".p", tri->numPoints(), (Vector3F *)tri->points());
	readIntData(".v", tri->numTriangles() * 3, (int *)tri->indices());
	
	return 1;
}

char HTriangleMesh::loadTriangleTexcoord(ATriangleMesh * tri)
{
	if(!hasNamedData(".tritexcoord")) {
	    std::cout<<"\n HTriangleMesh has no tritexcoord";
		return 0;
	}
	
	int nt = tri->numTriangles();
	readFloatData(".tritexcoord", nt * 6, tri->triangleTexcoords() );
	
	return 1;
}

}
//:~