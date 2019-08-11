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
#include <ctime>

namespace alo {
   
ThinPlateTest::ThinPlateTest() 
{
    std::time_t secs = std::time(0);
	Uniform<Lehmer> lmlcg(secs);
    
    m_mesh = new AdaptableMesh;
#if 0
    m_mesh->createMinimal();
#else    
    HeightField height;
    height.create(256, 256);
    height.setZero();
    height.setGridSize(1.345678f);
    height.setHeightScale(10.f);
    
    Polygon2D blasso;
    blasso<<Float2(-300.f, -300.f);
    blasso<<Float2(-300.f, 600.f);
    blasso<<Float2(600.f, 600.f);
    blasso<<Float2(600.f, -300.f);
    blasso.finish();
    
    Bilinear<float> bslope;
    bslope.value()[0] = .5e-3f;
    bslope.value()[1] = .5e-3f;
    bslope.value()[2] = .5e-3f;
    bslope.value()[3] = .5e-3f;
    
    Polygon2D flasso;
    flasso<<Float2(-150.f, -200.f);
    flasso<<Float2(-350.f, 10.f);
    flasso<<Float2(90.f, 250.f);
    flasso<<Float2(119.f, 150.f);
    flasso<<Float2(149.f, 160.f);
    flasso<<Float2(179.f, 199.f);
    flasso<<Float2(209.f, 179.f);
    flasso<<Float2(250.f, 190.f);
    flasso<<Float2(100.f, -200.f);
    flasso.finish();
    
    Bilinear<float> fslope;
    fslope.value()[0] = 5.f;
    fslope.value()[1] = .5f;
    fslope.value()[2] = 0.f;
    fslope.value()[3] = 15.5f;
    
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

#endif

#if 1
    BoundingBox shapeBox;
    m_mesh->getAabb(shapeBox);
    const float span = shapeBox.getLongestDistance();
    shapeBox.expand(span * .001f);
    shapeBox.round();
    std::cout<<"\n shape box"<<shapeBox;
    
    const float ssz = span * .001f;
	std::cout << "\n sample size " << ssz;
	typedef smp::Triangle<SurfaceSample > SamplerTyp;
    
    SamplerTyp sampler;
	sampler.setSampleSize(ssz);
    
    SurfaceSample asmp;
    SampleInterp<SurfaceSample> interp;
    
    typedef alo::sds::SpaceFillingVector<SurfaceSample> PntArrTyp;
	PntArrTyp pnts;
    
    const int nt = m_mesh->numTriangles();
    for(int i=0;i<nt;++i) {
        m_mesh->getTriangle<SurfaceSample, SamplerTyp >(sampler, i);
        sampler.addSamples <PntArrTyp, SampleInterp<SurfaceSample>, Uniform<Lehmer> >(pnts, asmp, interp, &lmlcg);
    }
    
    std::cout<<"\n n samples "<<pnts.size();
    
    sds::FZOrderCurve sfc;
    
    typedef sdf::SsdfBuildRule<sds::FZOrderCurve> BuildRuleTyp;
    BuildRuleTyp rule(&sfc);
    
    typedef sdf::SsdfBuilder<SurfaceSample, float, 4, 7, BuildRuleTyp > BuilderTyp;
	BuilderTyp builder;
    
    typedef sdf::SsdField FieldTyp;
    FieldTyp field;
    
    rule.setBBox(shapeBox);
    builder.build(&pnts, rule);
	builder.save<FieldTyp>(field, rule);
    
    ver1::H5IO hio;
	bool stat = hio.begin("./thin_plate.hes", HDocument::oCreate );
    
    ver1::HBase ga("/asset");
    
    float grdBox[6] = {1e10f, 1e10f, 1e10f, -1e10f, -1e10f, -1e10f};
	field.expandAabb(grdBox);
    
    HSsdf writer("/asset/fld0000");
	writer.save<FieldTyp>(field);
	writer.close();
    
    typedef grd::LocalGridBuildRule<sds::FZOrderCurve> LocBuildTyp;
	LocBuildTyp locRule(&sfc);
	locRule.setBBox(grdBox);
    locRule.setP(5);
    
    grd::LocalGrid<float> locG;
	locG.create(1<<locRule.P());
	locG.setAabb(grdBox);
    
    grd::LocalGridBuilder<grd::LocalGrid<float> > locBuilder;
	locBuilder.attach(&locG);
    
    grd::GridCellSamplesTyp cells;
    cells.create<FieldTyp>(&field);
    
    typedef sds::SpaceFillingVector<grd::GridCellSamplesTyp::SampleTyp> OutSampleTyp;
	const OutSampleTyp &orignalSamples = cells.samples();
    
	locBuilder.measure<OutSampleTyp, LocBuildTyp>(orignalSamples, 0, locRule);

    locBuilder.detach();

	HLocalGrid hlocG("/asset/localGrid");
	hlocG.save<grd::LocalGrid<float> >(locG);
	hlocG.close();

    HSurfaceSample hsurf("/asset/surf");
    hsurf.save<PntArrTyp>(pnts);
    hsurf.close();
    
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

}
