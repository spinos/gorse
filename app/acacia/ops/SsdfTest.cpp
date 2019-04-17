/*
 *  SsdfTest.cpp
 *  aloe
 *
 *  Created by zhang on 18-2-21.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#include "SsdfTest.h"
#include <ssdf/SparseSignedDistanceField.h>
#include <sds/FZOrder.h>
#include <math/BoundingBox.h>
#include <ogl/ShapeDrawer.h>
#include "PosSample.h"
#include <h5_ssdf/HSsdfIO.h>
#include <smp/Triangle.h>
#include <sds/SpaceFillingVector.h>
#include <sds/FZOrder.h>
#include <rng/Lehmer.h>
#include <rng/Uniform.h>
#include <geom/GeodesicSphere.h>

using namespace alo;

SsdfTest::SsdfTest()
{
	m_sfc = new SfcTyp;
	m_buildRule = new BuildRuleTyp(m_sfc);
	m_builder = new BuilderTyp;
	m_field = new FieldTyp;
}

void SsdfTest::buildSsdf(alo::sds::SpaceFillingVector<PosSample >* samples,
                const alo::BoundingBox& b)
{
	const Vector3F midP = b.center();
	const float spanL = b.getLongestDistance();
	m_sfc->setCoord(midP.x, midP.y, midP.z, spanL * .5f);
    m_builder->build(samples, *m_buildRule);
	m_builder->save<FieldTyp>(*m_field, *m_buildRule);
	saveToFile("../data/foo.aloe");
}

void SsdfTest::saveToFile(const char* filename)
{
	HSsdfIO hio;
	bool stat = hio.begin(filename, HDocument::oCreate );
	if(!stat) return;
	
	hio.saveSsdf<FieldTyp>(*m_field, "foo");
	
	hio.end();
}

void SsdfTest::drawGraph()
{
	drawGraph(m_builder->distanceField(), -1e20, 0.1, true, false, false);
	
}

void SsdfTest::drawGraph(const BaseDistanceField& fld, float zmin, float zmax, 
						bool drawEdge, bool drawDist, bool drawNormal)
{
	const int& nv = fld.numNodes();
	if(nv<1) return;
	const int& ne = fld.numEdges();
	const BaseDistanceField::NodeType* dns = fld.nodes();
	const BaseDistanceField::EdgeType* des = fld.edges();
	float boxCoord[4];
	boxCoord[3] = m_sfc->_h * 1.733f;
	float distScal = m_sfc->_h * 1200;
	
	ShapeDrawer::BeginLines();
	
	for(int i=0;i<nv;++i) {
		const BaseDistanceField::NodeType& ni = dns[i];
		
		if(ni.val > m_sfc->_h * 16)
			continue;
			
		if(ni.pos.z > zmax || ni.pos.z < zmin)
			continue;
		
		float r = ni.val / distScal;
		if(r > 1.f) r = 1.f;
		if(r > 0.f)
			ShapeDrawer::SetColor(r* .1f, .1f + r, 0.f);
			//continue;
		else
			ShapeDrawer::SetColor(.4f - .4f * r, .1f, 0.f);
			
		//if(ni.label == sdf::StFront)
		//	ShapeDrawer::LightYellow();
		
		memcpy(boxCoord, &ni.pos, 12);
		ShapeDrawer::BoundingBox(boxCoord);
		
		if(Absolute<float>(ni.val) < (m_sfc->_h * (1<<(10-5))) && drawDist) {
			Vector3F le = dns[ni._origin].pos;
			ShapeDrawer::Line((const float*)&ni.pos, (const float*)&le);
		}
		
		if(!drawNormal)
		    continue;
		
		Vector3F nml;
		if(fld.estimateNodeNormal(nml, i)) {

			ShapeDrawer::DarkGreen();
			nml = ni.pos + nml * .1f;
			ShapeDrawer::Line((const float*)&ni.pos, (const float*)&nml);
			
		}

	}
	ShapeDrawer::End();

	if(!drawEdge)
		return;
		
	ShapeDrawer::DarkRed();
    ShapeDrawer::BeginLines();

	for(int i=0;i<ne;++i) {
		
		const BaseDistanceField::EdgeType& ei = des[i];
		
		if(dns[ei.vi.x].pos.z > zmax
			|| dns[ei.vi.x].pos.z < zmin)
			continue;
			
		if(ei.lab > sdf::StFront)
			ShapeDrawer::DarkRed();
		else
			ShapeDrawer::DarkGreen();
		
		ShapeDrawer::Line((const float* )&dns[ei.vi.x].pos, 
						(const float* )&dns[ei.vi.y].pos);
		
	}	
	ShapeDrawer::End();
}

void SsdfTest::drawSamples()
{
	const sds::SpaceFillingVector<PosSample>& samps = m_builder->finestSample();
	const int n = samps.size();
	if(n<0)
		return;
		
	ShapeDrawer::DarkBlue();
	ShapeDrawer::BeginPoints();
	for(int i=0;i<n;++i) {
		if(samps[i]._pos.z > 0.f)
			continue;
		ShapeDrawer::Vertex((const float*)&samps[i]._pos);
	}
	ShapeDrawer::End();
}
/*
void measureShape()
{
	typedef alo::sds::SpaceFillingVector<PosSample> PntArrTyp;
	PntArrTyp* m_pnts = new PntArrTyp; 
    
    sds::FZOrderCurve* m_zfc = new sds::FZOrderCurve;
    GeodesicSphere* m_shape = new GeodesicSphere;
	
    BoundingBox shapeBox;
    m_shape->calculateGeomBBox(shapeBox);
    shapeBox.round();
    std::cout<<"\n shape box"<<shapeBox;
    
    m_pnts->clear();
    
    const float ssz = shapeBox.getLongestDistance() * .00141f;
	
	typedef smp::Triangle<PosSample > SamplerTyp;
   
	SamplerTyp sampler;
	sampler.setSampleSize(ssz);
    
    PosSample asmp;
    SampleInterp interp;
	std::time_t secs = std::time(0);
	Uniform<Lehmer> lmlcg(secs);
    
    const int nt = m_shape->numTriangles();
    for(int i=0;i<nt;++i) {
        m_shape->dumpTriangle<PosSample, SamplerTyp >(sampler, i);
        sampler.addSamples <PntArrTyp, SampleInterp, Uniform<Lehmer> >(*m_pnts, asmp, interp, &lmlcg);
    }
    
    std::cout<<"\n n triangle samples "<<m_pnts->size();
    
	m_ssd->buildSsdf(m_pnts, shapeBox);

	std::cout.flush();
    
}*/
