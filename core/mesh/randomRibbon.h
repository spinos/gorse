#ifndef ALO_MESH_RANDOM_RIBBON_H
#define ALO_MESH_RANDOM_RIBBON_H

#include "FrontMesher.h"
#include <geom/AdaptableMesh.h>
#include <math/miscfuncs.h>

namespace alo {
	
inline void createRandomRibbon(AdaptableMesh &msh, int nu, int nv)
{
	for(int i=0;i<nu;++i)
        msh.addVertex(Vector3F(RandomFn11() * .13f + .53f * i, RandomFn11() * .19f, .3f * i));
    
    FrontLine originLine;
    FrontLine::EnableMerging = false;
    
    for(int i=0;i<nu;++i)
        originLine.addVertex(msh.vertexPositionR(i), i);
    
    Vector3F up(0.f, .41f, 0.f);
    const Quaternion lq(-.0572f, Vector3F::ZAxis);
    const Quaternion tq(.083f, Vector3F::YAxis);
    constexpr float cshrinking = .0f;
    originLine.setWorldSpace(Matrix33F::IdentityMatrix);
    originLine.rotateLocalBy(lq);
    originLine.rotateLocalBy(tq);
    originLine.setShrinking(cshrinking);
    originLine.setDirection(up);
    originLine.setMinEdgeLength(.2f);
    
    FrontLine *la = &originLine;
    FrontLine l[100];
    if(nv > 100) nv = 100;
    
    FrontMesher msher;
    msher.attachMesh(&msh);
    for(int i=0;i<nv;++i) {
        msher.setFrontId(i+1);

        l[i].rotateLocalBy(lq);
        l[i].rotateLocalBy(tq);
        l[i].setShrinking(cshrinking + RandomFn11() * .07f);
        up.y += RandomFn11() * .29f;
        up.z += RandomFn11() * .29f;
        l[i].setDirection(up);
        l[i].setMinEdgeLength(.2f);

        msher.advanceFront(l[i], *la);

        la = &l[i];
    }

    msh.calculateVertexNormals();
}

}

#endif
