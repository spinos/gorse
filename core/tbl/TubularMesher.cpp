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

void TubularMesher::triangulate(const TubularCrossSection &cs, const TubularProfile &prof)
{
	Vector3F q, center, disp;
	Matrix33F frm; 
	Float2 pitchYaw;
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

	fline.setWorldSpace(frm);
    fline.setDirection(disp);
	fline.setLocalRotation(pitchYaw);
    fline.setShrinking(0.f);
    
    const int ns = prof.numSegments();

    for(int i=1;i<ns;++i) {
    	prof.getSegment(center, frm, disp, pitchYaw, i);

    	setFrontId(i);

    	FrontLine &lb = line[i&1];
    	lb.clearLine();
    	lb.setWorldSpace(frm);
    	lb.setDirection(disp);
    	lb.setLocalRotation(pitchYaw);
		lb.setShrinking(0.f);
    	
    	FrontLine &la = line[(i-1)&1];
    	
    	advanceFront(lb, la);
    }

    FrontLine &lline = line[(ns-1)&1];
    FrontLine &tline = line[ns&1];
    tline.clearLine();
    advanceFront(tline, lline);
    
}

}
