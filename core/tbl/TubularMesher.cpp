/*
 *  TubularMesher.cpp
 *  gorse
 *
 *  2019/8/10
 */

#include "TubularMesher.h"
#include <geom/AdaptableMesh.h>
#include "TubularCrossSection.h"
#include "TubularProfile.h"
#include <mesh/FrontLine.h>

namespace alo {

TubularMesher::TubularMesher()
{}

TubularMesher::~TubularMesher()
{}

void TubularMesher::triangulate(const TubularCrossSection &cs, const TubularProfile &prof,
                                Parameter *param)
{
	Vector3F q, center, disp, realDisp;
    float shrk, realShrink;
	Matrix33F frm; 
	Float2 pitchYaw, realPitchYaw;
	prof.getSegment(center, frm, disp, pitchYaw, 0);

	AdaptableMesh *msh = mesh();

	const int voffset = msh->numVertices();

	const int csnu = cs.numVertices();
	for(int i=0;i<csnu;++i) {
		cs.getVertex((float *)&q, i);
		frm.transformInPlace(q);
		q += center;
		msh->addVertex(q);
    }

	FrontLine line[2];
	FrontLine &fline = line[0];
	fline.clearLine();
	for(int i=0;i<csnu;++i) {
		fline.addVertex(msh->vertexPositionR(voffset + i), voffset + i);
	}
	fline.closeLine();

	FrontLine::EnableMerging = false;
    
    float lastFrac = 1.f;
    int ns = prof.numSegments();
    if(param != nullptr) {
        ns = param->_toSegment;
        lastFrac = param->_toSegment - ns;
        if(lastFrac > 0.01f) ns++;
        
        if(ns > prof.numSegments()) {
            ns = prof.numSegments();
            lastFrac = 1.f;
        }
    }
    
    realDisp = disp;
    realPitchYaw = pitchYaw;
    shrk = prof.getShrinkingFactor(0);
    realShrink = shrk;
    if(ns == 1 && lastFrac < .99f) {
        realDisp *= lastFrac;
        realPitchYaw.set(pitchYaw.x * lastFrac, pitchYaw.y * lastFrac);
        realShrink *= lastFrac;
    }

	fline.setWorldSpace(frm);
    fline.setDirection(realDisp);
	fline.setLocalRotation(realPitchYaw);
    fline.setShrinking(realShrink);

    for(int i=1;i<ns;++i) {
    	prof.getSegment(center, frm, disp, pitchYaw, i);

    	setFrontId(i);

    	FrontLine &lb = line[i&1];
    	lb.clearLine();
    	lb.setWorldSpace(frm);
        
        realDisp = disp;
        realPitchYaw = pitchYaw;
        shrk = prof.getShrinkingFactor(i);
        realShrink = shrk;
        if(i == ns - 1 && lastFrac < .99f) {
            realDisp *= lastFrac;
            realPitchYaw.set(pitchYaw.x * lastFrac, pitchYaw.y * lastFrac);
            realShrink *= lastFrac;
        }
        
    	lb.setDirection(realDisp);
    	lb.setLocalRotation(realPitchYaw);
		lb.setShrinking(realShrink);
    	
    	FrontLine &la = line[(i-1)&1];
    	
    	advanceFront(lb, la);
    }

    FrontLine &lline = line[(ns-1)&1];
    FrontLine &tline = line[ns&1];
    tline.clearLine();
    advanceFront(tline, lline);
    
}

}
