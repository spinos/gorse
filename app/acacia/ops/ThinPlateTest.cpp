/*
 *  ThinPlateTest.cpp
 *  acacia
 *
 */

#include "ThinPlateTest.h"
#include <qt_ogl/DrawableScene.h>
#include <qt_ogl/DrawableObject.h>
#include <qt_ogl/DrawableResource.h>
#include <sds/SpaceFillingVector.h>
#include <sds/FZOrder.h>
#include <sds/SparseFieldBuilder.h>
#include <sds/SparseFieldBuildRule.h>
#include <htf/HeightFieldMesher.h>
#include <math/Polygon2D.h>
#include <math/Bilinear.h>
#include <rng/Lehmer.h>
#include <rng/Uniform.h>
#include <smp/Triangle.h>
#include <smp/SurfaceSample.h>
#include <ssdf/SsdField.h>
#include <ssdf/SsdfBuilder.h>
#include <ssdf/SsdfBuildRule.h>
#include <grd/LocalGrid.h>
#include <grd/LocalGridBuilder.h>
#include <grd/LocalGridBuildRule.h>
#include <grd/GridSamples.h>
#include <h5/V1H5IO.h>
#include <h5_ssdf/HSsdf.h>
#include <h5_grd/HLocalGrid.h>
#include <h5_smp/HSurfaceSample.h>
#include <math/Matrix33F.h>
#include <mesh/Fissure.h>
#include <bvh/BVHNodeIterator.h>
#include <boost/format.hpp>
#include <boost/chrono/include.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <ctime>

namespace alo {
    
boost::mutex ThinPlateTest::sMutex;
   
ThinPlateTest::ThinPlateTest() 
{
    std::time_t secs = std::time(0);
	Uniform<Lehmer> lmlcg(secs);
    
    m_mesh = new AdaptableMesh;
   
    HeightField height;
    height.create(256, 256);
    height.setZero();
    height.setGridSize(7.345678f);
    height.setHeightScale(79.f);
    
    const float regionBound = 3000.f;
    Polygon2D blasso;
    blasso<<Float2(-regionBound, -regionBound);
    blasso<<Float2(-regionBound, regionBound);
    blasso<<Float2(regionBound, regionBound);
    blasso<<Float2(regionBound, -regionBound);
    blasso.finish();
    
    Bilinear<float> bslope;
    bslope.value()[0] = 4.5f + .5e-3f;
    bslope.value()[1] = -.5f + .5e-3f;
    bslope.value()[2] = 0.f + .5e-3f;
    bslope.value()[3] = -1.5f + .5e-3f;
    
    Polygon2D flasso;
    flasso<<Float2(-100.f, -100.f);
    flasso<<Float2(100.f, 110.f);
    flasso<<Float2(90.f, 210.f);
    flasso<<Float2(190.f, 200.f);
    flasso<<Float2(229.f, 300.f);
    flasso<<Float2(490.f, 239.f);
    flasso<<Float2(400.f, 579.f);
    flasso<<Float2(650.f, 500.f);
    flasso<<Float2(500.f, 800.f);
    flasso<<Float2(700.f, 1200.f);
    flasso<<Float2(900.f, 1300.f);
    flasso<<Float2(1200.f, 1000.f);
    flasso<<Float2(1550.f, 1050.f);
    flasso<<Float2(1600.f, 650.f);
    flasso<<Float2(1750.f, 550.f);
    flasso<<Float2(1850.f, 250.f);
    flasso<<Float2(1950.f, 0.f);
    flasso.finish();
    
    Bilinear<float> fslope;
    fslope.value()[0] = 5.f;
    fslope.value()[1] = .5f;
    fslope.value()[2] = 1.5f;
    fslope.value()[3] = 8.5f;
    
    height.setValue(bslope, blasso);
    height.setValue(fslope, flasso);
    height.smooth();
    height.smooth();
    height.smooth();
    
    HeightFieldMesher heighter;
    heighter.attach(m_mesh);
    heighter.triangulate(height);
    //Quaternion rot(-.009f, Vector3F::YAxis);
    //Matrix33F mrot(rot);
    //m_mesh->rotateBy(mrot);
    heighter.detach();
    
    Fissure fis;
    const int npart = fis.granulate(m_mesh);
    std::cout << " granulate to " << npart << " parts ";

#if 1
    ver1::H5IO hio;
	bool stat = hio.begin("./thin_plate.hes", HDocument::oCreate );
    
    ver1::HBase ga("/asset");
    
    float grdBox[6] = {1e10f, 1e10f, 1e10f, -1e10f, -1e10f, -1e10f};
    std::vector<sdf::SsdField *> fields;
	fields.resize(npart);
    
    boost::chrono::system_clock::time_point t0 = boost::chrono::system_clock::now();
    
    AdaptableMesh *transient[8];
	for(int i=0;i<8;++i) transient[i] = new AdaptableMesh;
	
	boost::thread tref[8];
    int ntref = 0;
    
    int mi=0;
	BVHNodeIterator it = fis.firstPart();
    while(it._node) {
        
        fis.reformPart(transient[ntref], it, m_mesh);
		
		tref[ntref] = boost::thread(boost::bind(&ThinPlateTest::WriteField, _1, _2, _3, _4), 
												transient[ntref], mi, &fields, grdBox);
		ntref++;
		
		if(ntref==8) {
            for(int i=0;i<ntref;++i)
                tref[i].join();
            ntref = 0;
        }
		
        it = fis.nextPart(it);
		mi++;
    }
	
	if(ntref>0) {
        for(int i=0;i<ntref;++i)
            tref[i].join();
    }
	
	for(int i=0;i<8;++i) delete transient[i];
    
    WriteGrid(fields, grdBox);
    
    WriteTerrainSamples(m_mesh, grdBox);
    
    boost::chrono::system_clock::time_point t1 = boost::chrono::system_clock::now();

    boost::chrono::duration<double> sec = t1 - t0;
    std::cout << "\n terrain finished in " << sec.count() << " seconds ";
    
    ga.close();
	hio.end();
#endif
    
    DrawableResource *rec = createResource();
    setResource(rec);
}

ThinPlateTest::~ThinPlateTest()
{}
    
void ThinPlateTest::update()
{
    computeMesh();
}

void ThinPlateTest::addDrawableTo(DrawableScene *scene)
{
    setDrawableScene(scene);
    computeMesh();  
}

void ThinPlateTest::computeMesh()
{
    DrawableResource *rec = resource();

    lockScene();
    const int beforeFrame = frameNumber() - 2;
    if(rec->changedOnFrame() > beforeFrame) {
/// prevent editing unsynchronized resource
        unlockScene();
        return;
    }

    UpdateMeshResouce(rec, m_mesh);

    processResourceNoLock(rec);
    unlockScene();

    setBound(m_mesh);
}

bool ThinPlateTest::WriteField(const AdaptableMesh *msh, 
				const int &grpId,
				std::vector<sdf::SsdField *> *fields,
				float *grdBox)
{
    std::time_t secs = std::time(0);
	Uniform<Lehmer> lmlcg(secs);
    
    BoundingBox shapeBox;
    msh->getAabb(shapeBox);
    const float span = shapeBox.getLongestDistance();
    shapeBox.expand(span * .001f);
    shapeBox.round();
    
    const float ssz = span * .001f;
	
	typedef smp::Triangle<SurfaceSample > SamplerTyp;
    
    SamplerTyp sampler;
	sampler.setSampleSize(ssz);
    
    SurfaceSample asmp;
    SampleInterp<SurfaceSample> interp;
    
    typedef alo::sds::SpaceFillingVector<SurfaceSample> PntArrTyp;
	PntArrTyp pnts;
    
    const int nt = msh->numTriangles();
    for(int i=0;i<nt;++i) {
        msh->getTriangle<SurfaceSample, SamplerTyp >(sampler, i);
        sampler.addSamples <PntArrTyp, SampleInterp<SurfaceSample>, Uniform<Lehmer> >(pnts, asmp, interp, &lmlcg);
    }
    
    sds::FZOrderCurve sfc;
    
    typedef sdf::SsdfBuildRule<sds::FZOrderCurve> BuildRuleTyp;
    BuildRuleTyp rule(&sfc);
    
    typedef sdf::SsdfBuilder<SurfaceSample, float, 5, 7, BuildRuleTyp > BuilderTyp;
	BuilderTyp builder;
    
    typedef sdf::SsdField FieldTyp;
    FieldTyp *field = new FieldTyp;
    
    rule.setBBox(shapeBox);
    builder.build(&pnts, rule);
	builder.save<FieldTyp>(*field, rule);
    
    boost::lock_guard<boost::mutex> lock(sMutex);
    
    std::cout<<"\n shape box"<<shapeBox
        << "\n sample size " << ssz
        <<"\n n samples "<<pnts.size();
        
	field->expandAabb(grdBox);
    
    const std::string idName = boost::str(boost::format("%04i") % grpId);
	const std::string fullName = boost::str(boost::format("/asset/fld%1%") % idName );

	HSsdf writer(fullName);
	writer.save<FieldTyp>(*field);
	writer.close();
	
	(*fields)[grpId] = field;
		
	return true;
    
}

bool ThinPlateTest::WriteGrid(const std::vector<sdf::SsdField *> &fields,
				const float *grdBox)
{
    sds::FZOrderCurve sfc;
    
    typedef grd::LocalGridBuildRule<sds::FZOrderCurve> LocBuildTyp;
	LocBuildTyp locRule(&sfc);
	locRule.setBBox(grdBox);
    locRule.setP(5);
    
    grd::LocalGrid<float> locG;
	locG.create(1<<locRule.P());
	locG.setAabb(grdBox);
    
    grd::LocalGridBuilder<grd::LocalGrid<float> > locBuilder;
	locBuilder.attach(&locG);
    
    typedef sdf::SsdField FieldTyp;
    
    grd::GridCellSamplesTyp cells;
    typedef sds::SpaceFillingVector<grd::GridCellSamplesTyp::SampleTyp> OutSampleTyp;
	
    std::vector<FieldTyp *>::const_iterator it = fields.begin();
	for(int i=0;it!=fields.end();++it,++i) {
		cells.create<FieldTyp>(*it);
        
        const OutSampleTyp &orignalSamples = cells.samples();
        locBuilder.measure<OutSampleTyp, LocBuildTyp>(orignalSamples, i, locRule);

    }

    locBuilder.detach();

	HLocalGrid hlocG("/asset/localGrid");
	hlocG.save<grd::LocalGrid<float> >(locG);
	hlocG.close();
	
	return true;
}

bool ThinPlateTest::WriteTerrainSamples(const AdaptableMesh *msh, 
                const float *grdBox)
{
    std::time_t secs = std::time(0);
	Uniform<Lehmer> lmlcg(secs);
    
    BoundingBox shapeBox(grdBox);
    const float span = shapeBox.getLongestDistance();
    const float ssz = span * .002f;
	
	typedef smp::Triangle<SurfaceSample > SamplerTyp;
    
    SamplerTyp sampler;
	sampler.setSampleSize(ssz);
    
    SurfaceSample asmp;
    SampleInterp<SurfaceSample> interp;
    
    typedef alo::sds::SpaceFillingVector<SurfaceSample> PntArrTyp;
	PntArrTyp pnts;
    
    const int nt = msh->numTriangles();
    for(int i=0;i<nt;++i) {
        msh->getTriangle<SurfaceSample, SamplerTyp >(sampler, i);
        sampler.addSamples <PntArrTyp, SampleInterp<SurfaceSample>, Uniform<Lehmer> >(pnts, asmp, interp, &lmlcg);
    }
    
    HSurfaceSample hsurf("/asset/surf");
    hsurf.save<PntArrTyp>(pnts);
    hsurf.close();
    
    return true;
}

}
