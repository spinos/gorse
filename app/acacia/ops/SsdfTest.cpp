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
#include <h5/V1H5IO.h>
#include <smp/Triangle.h>
#include <smp/SurfaceSample.h>
#include <sds/SpaceFillingVector.h>
#include <sds/FZOrder.h>
#include <rng/Lehmer.h>
#include <rng/Uniform.h>
#include <geom/GeodesicSphere.h>
#include <mesh/randomRibbon.h>
#include <boost/format.hpp>
#include <h5_ssdf/HSsdf.h>
#include <grd/IndexGrid.h>
#include <grd/IndexGridBuilder.h>
#include <grd/IndexGridBuildRule.h>
#include <h5_ssdf/HIndexGrid.h>
#include <QProgressDialog>
#include <QApplication>

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
}

SsdfTest::~SsdfTest()
{
	delete m_sfc;
	delete m_buildRule;
	delete m_builder;
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
	for(int i=0;i<8;++i) {
		AdaptableMesh *transient = new AdaptableMesh;
		createRandomRibbon(*transient, 6, 97);
		m_ribbon[i] = transient;
	}

	setDrawableSize(8);

	lockScene();
    const int n = numResources();
    for(int i=0;i<n;++i) {
        DrawableResource *rec = resource(i);
        UpdateMeshResouce(rec, m_ribbon[i]);
        processResourceNoLock(rec);
    }
    unlockScene();

}

void SsdfTest::buildSsdf(sds::SpaceFillingVector<SurfaceSample >* samples,
                FieldTyp *field, const BoundingBox& b)
{
	m_buildRule->setBBox(b);
    m_builder->build(samples, *m_buildRule);
	m_builder->save<FieldTyp>(*field, *m_buildRule);
}

void SsdfTest::saveToFile(const std::string &filename)
{
	QProgressDialog progress("Processing...", QString(), 0, 16, QApplication::activeWindow() );
    progress.setWindowModality(Qt::ApplicationModal);
    progress.show();

	ver1::H5IO hio;
	bool stat = hio.begin(filename, HDocument::oCreate );
	if(!stat) return;

	ver1::HBase ga("/asset");

	FieldTyp *fields[8];
	float grdBox[6] = {1e10f, 1e10f, 1e10f, -1e10f, -1e10f, -1e10f};
	
	for(int i=0;i<8;++i) {
		fields[i] = new FieldTyp;

		buildField(fields[i], *m_ribbon[i]);
		fields[i]->expandAabb(grdBox);

		std::string fullName = boost::str(boost::format("/asset/foo_%1%") % i );

		HSsdf writer(fullName);
		writer.save<FieldTyp>(*fields[i]);
		writer.close();
		progress.setValue(i);
	}

	grd::IndexGrid indG;
	indG.setResolution(1<<4);
	indG.setAabb(grdBox);

	grd::IndexGridBuilder<4> indBuilder;
	indBuilder.attach(&indG);

typedef grd::IndexGridBuildRule<sds::FZOrderCurve> IndBuildTyp;
	IndBuildTyp indRule(m_sfc);
	indRule.setBBox(grdBox);
	indRule.setBoxRelativeBoundary(1.f);

typedef sdf::SsdfLookupRule<FieldTyp> IndSampleTyp;
	IndSampleTyp indSample;

	for(int i=0;i<8;++i) {
		progress.setValue(8+i);
		indSample.attach(*fields[i]);
		indBuilder.measure<IndSampleTyp, IndBuildTyp>(indSample, i, indRule);

	}

	for(int i=0;i<8;++i) {
		delete fields[i];
	}

	indBuilder.detach();

	HIndexGrid hindG("/asset/indexGrid");
	hindG.save(indG);
	hindG.close();
	
	ga.close();
	
	hio.end();
	progress.setValue(16);
}

void SsdfTest::buildField(FieldTyp *field, const AdaptableMesh &transient)
{
	typedef alo::sds::SpaceFillingVector<SurfaceSample> PntArrTyp;
	PntArrTyp pnts;
    
    BoundingBox shapeBox;
    transient.getAabb(shapeBox);
    shapeBox.round();
    std::cout<<"\n shape box"<<shapeBox;
    
    const float ssz = shapeBox.getLongestDistance() * .001f;
	std::cout << "\n sample size " << ssz;
	typedef smp::Triangle<SurfaceSample > SamplerTyp;
   
	SamplerTyp sampler;
	sampler.setSampleSize(ssz);
    
    SurfaceSample asmp;
    SampleInterp interp;
	std::time_t secs = std::time(0);
	Uniform<Lehmer> lmlcg(secs);
    
    const int nt = transient.numTriangles();
    for(int i=0;i<nt;++i) {
        transient.getTriangle<SurfaceSample, SamplerTyp >(sampler, i);
        sampler.addSamples <PntArrTyp, SampleInterp, Uniform<Lehmer> >(pnts, asmp, interp, &lmlcg);
    }
    
    std::cout<<"\n n samples "<<pnts.size();
    buildSsdf(&pnts, field, shapeBox);
}

bool SsdfTest::hasMenu() const
{ return true; }

void SsdfTest::getMenuItems(std::vector<std::pair<std::string, int > > &ks) const
{ ks.push_back(std::make_pair("Save", AFileDlgProfile::FWrite)); }

void SsdfTest::recvAction(int x)
{ if(x == AFileDlgProfile::FWrite) saveToFile(SWriteProfile.getFilePath()); }

AFileDlgProfile *SsdfTest::writeFileProfileR () const
{
	SWriteProfile._notice = boost::str(boost::format("sparse signed distance field P %1% Q %2% \n n mesh %3%") 
        % 5 % 7 % 8 );
    return &SWriteProfile; 
}

} /// end of namespace alo
