/*
 *  ABox.cpp
 *  
 *
 *  Created by jian zhang on 2/4/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */

#include "ABox.h"
#ifdef LINUX
#include <string.h>
#endif

namespace alo {

const unsigned ABox::BoxFaceIndices[] = {
0,1,2,
0,2,3,
4,5,7,
5,6,7,
8,9,10,
8,10,11,
12,13,15,
13,14,15,
16,17,18,
16,18,19,
20,21,23,
21,22,23
};

const float ABox::BoxVertexNormals[] = {
-1.f, 0.f, 0.f,
-1.f, 0.f, 0.f,
-1.f, 0.f, 0.f,
-1.f, 0.f, 0.f,
 1.f, 0.f, 0.f,
 1.f, 0.f, 0.f,
 1.f, 0.f, 0.f,
 1.f, 0.f, 0.f,
 0.f,-1.f, 0.f,
 0.f,-1.f, 0.f,
 0.f,-1.f, 0.f,
 0.f,-1.f, 0.f,
 0.f, 1.f, 0.f,
 0.f, 1.f, 0.f,
 0.f, 1.f, 0.f,
 0.f, 1.f, 0.f,
 0.f, 0.f,-1.f,
 0.f, 0.f,-1.f,
 0.f, 0.f,-1.f,
 0.f, 0.f,-1.f,
 0.f, 0.f, 1.f,
 0.f, 0.f, 1.f,
 0.f, 0.f, 1.f,
 0.f, 0.f, 1.f,
};

ABox::ABox() 
{
    const int np = 24;
    const int nt = 12;
    createTriangleMesh(np, nt);
    updateBox(BoundingBox(-1.f, -1.f, -1.f, 1.f, 1.f, 1.f));
    unsigned *ind = indices();
    memcpy(ind, BoxFaceIndices, 12 * 3 * 4);
    Vector3F* nmls = normals();
    memcpy(nmls, BoxVertexNormals, 24 * 3 * 4);
}

void ABox::updateBox(const BoundingBox &box)
{
    Vector3F *p = positions();
    Vector3F bv = box.getMin();
    p[0] = p[8] = p[16] = bv;
    
    bv.set(box.getMin(0), box.getMin(1), box.getMax(2));
    p[1] = p[11] = p[20] = bv;
    
    bv.set(box.getMin(0), box.getMax(1), box.getMax(2));
    p[2] = p[13] = p[23] = bv;
    
    bv.set(box.getMin(0), box.getMax(1), box.getMin(2));
    p[3] = p[12] = p[17] = bv;
    
    bv.set(box.getMax(0), box.getMin(1), box.getMin(2));
    p[4] = p[9] = p[19] = bv;
    
    bv.set(box.getMax(0), box.getMax(1), box.getMin(2));
    p[5] = p[15] = p[18] = bv;
    
    bv = box.getMax();
    p[6] = p[14] = p[22] = bv;
    
    bv.set(box.getMax(0), box.getMin(1), box.getMax(2));
    p[7] = p[10] = p[21] = bv;
}

}