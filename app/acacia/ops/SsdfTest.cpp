/*
 *  SsdfTest.cpp
 *  aloe
 *
 *  Created by zhang on 18-2-21.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#include "SsdfTest.h"
#include <qt_base/AFileDlg.h>
#include <qt_ogl/DrawableResource.h>
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

namespace alo {

AFileDlgProfile SsdfTest::SWriteProfile(AFileDlgProfile::FWrite,
        "Choose File To Save",
        ":images/save_big.png",
        "Save sparse signed distance field",
        "Save .hes",
        ".hes",
        "./",
        "untitled");

SsdfTest::SsdfTest()
{
	m_sfc = new SfcTyp;
	m_buildRule = new BuildRuleTyp(m_sfc);
	m_builder = new BuilderTyp;
	m_field = new FieldTyp;
	testIt();

	DrawableResource *rec = createResource();
    setResource(rec);
}

SsdfTest::~SsdfTest()
{
	delete m_sfc;
	delete m_buildRule;
	delete m_builder;
	delete m_field;
}

void SsdfTest::update()
{}

void SsdfTest::addDrawableTo(DrawableScene *scene)
{
    setDrawableScene(scene);
    computeMesh();  
}

void SsdfTest::computeMesh()
{
	GeodesicSphere transient(7);
    transient.scaleBy(9.f);
    DrawableResource *rec = resource();

    lockScene();
    const int beforeFrame = frameNumber() - 2;
    if(rec->changedOnFrame() > beforeFrame) {
/// prevent editing unsynchronized resource
        unlockScene();
        return;
    }

    UpdateMeshResouce(rec, &transient, false);

    processResourceNoLock(rec);
    unlockScene();
}

void SsdfTest::buildSsdf(sds::SpaceFillingVector<PosSample >* samples,
                const BoundingBox& b)
{
	const Vector3F midP = b.center();
	const float spanL = b.getLongestDistance();
	m_sfc->setCoord(midP.x, midP.y, midP.z, spanL * .5f);
    m_builder->build(samples, *m_buildRule);
	m_builder->save<FieldTyp>(*m_field, *m_buildRule);
	//saveToFile("../data/foo.aloe");
}

void SsdfTest::saveToFile(const std::string &filename)
{
	HSsdfIO hio;
	bool stat = hio.begin(filename, HDocument::oCreate );
	if(!stat) return;
	
	hio.saveSsdf<FieldTyp>(*m_field, "foo");
	
	hio.end();
}

void SsdfTest::testIt()
{
	typedef alo::sds::SpaceFillingVector<PosSample> PntArrTyp;
	PntArrTyp pnts; 
    
    GeodesicSphere transient(10);
    transient.scaleBy(8.f);
	
    BoundingBox shapeBox;
    transient.getAabb(shapeBox);
    shapeBox.round();
    std::cout<<"\n shape box"<<shapeBox;
    
    const float ssz = shapeBox.getLongestDistance() * .00141f;
	std::cout << "\n sample size " << ssz;
	typedef smp::Triangle<PosSample > SamplerTyp;
   
	SamplerTyp sampler;
	sampler.setSampleSize(ssz);
    
    PosSample asmp;
    SampleInterp interp;
	std::time_t secs = std::time(0);
	Uniform<Lehmer> lmlcg(secs);
    
    const int nt = transient.numTriangles();
    for(int i=0;i<nt;++i) {
        transient.getTriangle<PosSample, SamplerTyp >(sampler, i);
        sampler.addSamples <PntArrTyp, SampleInterp, Uniform<Lehmer> >(pnts, asmp, interp, &lmlcg);
    }
    
    std::cout<<"\n n triangle samples "<<pnts.size();
    buildSsdf(&pnts, shapeBox);
}

bool SsdfTest::hasMenu() const
{ return true; }

void SsdfTest::getMenuItems(std::vector<std::pair<std::string, int > > &ks) const
{ ks.push_back(std::make_pair("Save", AFileDlgProfile::FWrite)); }

void SsdfTest::recvAction(int x)
{ if(x == AFileDlgProfile::FWrite) saveToFile(SWriteProfile.getFilePath()); }

AFileDlgProfile *SsdfTest::writeFileProfileR () const
{
	SWriteProfile._notice = boost::str(boost::format("sparse signed distance field P %1% Q %2% \n storage size (%3%,%4%)") 
        % m_field->P() % m_field->Q() 
        % m_field->coarsDistanceStorageSize() % m_field->fineDistanceStorageSize() );
    return &SWriteProfile; 
}

}

/*void SsdfTest::drawGraph()
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
*/
