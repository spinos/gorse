/*
 *  SsdfTest.cpp
 *  aloe
 *
 *  2019/5/5
 */

#include "SsdfTest.h"
#include <qt_base/AFileDlg.h>
#include <qt_ogl/DrawableResource.h>
#include <ssdf/SsdField.h>
#include <ssdf/SsdfBuilder.h>
#include <ssdf/SsdfBuildRule.h>
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
#include <grd/LocalGrid.h>
#include <grd/LocalGridBuilder.h>
#include <grd/LocalGridBuildRule.h>
#include <grd/GridSamples.h>
#include <h5_grd/HLocalGrid.h>
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
	std::time_t secs = std::time(0);
	Uniform<Lehmer> lmlcg(secs);

	for(int i=0;i<NUM_RIBBONS;++i) {
		AdaptableMesh *transient = new AdaptableMesh;
		createRandomRibbon<Uniform<Lehmer> >(*transient, 6, 93, &lmlcg);
		m_ribbon[i] = transient;
	}

	setDrawableSize(NUM_RIBBONS);

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
	QProgressDialog progress("Processing...", QString(), 0, NUM_RIBBONS*2, QApplication::activeWindow() );
    progress.setWindowModality(Qt::ApplicationModal);
    progress.show();

	ver1::H5IO hio;
	bool stat = hio.begin(filename, HDocument::oCreate );
	if(!stat) return;

	ver1::HBase ga("/asset");

	FieldTyp *fields[NUM_RIBBONS];
	float grdBox[6] = {1e10f, 1e10f, 1e10f, -1e10f, -1e10f, -1e10f};
	
	for(int i=0;i<NUM_RIBBONS;++i) {
		fields[i] = new FieldTyp;

		buildField(fields[i], *m_ribbon[i]);
		fields[i]->expandAabb(grdBox);

		const std::string idName = boost::str(boost::format("%04i") % i);
		const std::string fullName = boost::str(boost::format("/asset/fld%1%") % idName );

		HSsdf writer(fullName);
		writer.save<FieldTyp>(*fields[i]);
		writer.close();
		progress.setValue(i);
	}
    
    typedef grd::LocalGridBuildRule<sds::FZOrderCurve> LocBuildTyp;
	LocBuildTyp locRule(m_sfc);
	locRule.setBBox(grdBox);
	locRule.setBoxRelativeBoundary(2.f);
    locRule.setP(5);

	grd::LocalGrid<float> locG;
	locG.create(1<<locRule.P());
	locG.setAabb(grdBox);

	grd::LocalGridBuilder<grd::LocalGrid<float> > locBuilder;
	locBuilder.attach(&locG);

	grd::PointGridSamplesTyp samples;

	for(int i=0;i<NUM_RIBBONS;++i) {
		progress.setValue(NUM_RIBBONS+i);

		samples.create<FieldTyp>(fields[i]);

		typedef sds::SpaceFillingVector<grd::PointGridSamplesTyp::SampleTyp> OutSampleTyp;
		const OutSampleTyp &orignalSamples = samples.samples();

		locBuilder.measure<OutSampleTyp, LocBuildTyp>(orignalSamples, i, locRule);

	}

	for(int i=0;i<NUM_RIBBONS;++i) {
		delete fields[i];
	}

	locBuilder.detach();

	HLocalGrid hlocG("/asset/localGrid");
	hlocG.save<grd::LocalGrid<float> >(locG);
	hlocG.close();
	
	ga.close();
	
	hio.end();
	progress.setValue(NUM_RIBBONS*2);
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
    SampleInterp<SurfaceSample> interp;
	std::time_t secs = std::time(0);
	Uniform<Lehmer> lmlcg(secs);
    
    const int nt = transient.numTriangles();
    for(int i=0;i<nt;++i) {
        transient.getTriangle<SurfaceSample, SamplerTyp >(sampler, i);
        sampler.addSamples <PntArrTyp, SampleInterp<SurfaceSample>, Uniform<Lehmer> >(pnts, asmp, interp, &lmlcg);
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
        % 5 % 7 % NUM_RIBBONS );
    return &SWriteProfile; 
}

} /// end of namespace alo
