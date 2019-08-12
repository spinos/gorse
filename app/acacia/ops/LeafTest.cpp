/*
 *  LeafTest.cpp
 *  acacia
 *
 */

#include "LeafTest.h"
#include <qt_ogl/DrawableScene.h>
#include <qt_ogl/DrawableObject.h>
#include <qt_ogl/DrawableResource.h>
#include <sds/SpaceFillingVector.h>
#include <sds/FZOrder.h>
#include <sds/SparseFieldBuilder.h>
#include <sds/SparseFieldBuildRule.h>
#include <geom/AdaptableMesh.h>
#include <rng/Lehmer.h>
#include <rng/Uniform.h>
#include <smp/Triangle.h>
#include <smp/SurfaceGeodesicSample.h>
#include <sds/SparseCubicField.h>
#include <gdf/GeodesicDistance.h>
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
#include <mesh/RegionMesher.h>
#include <morph/LeafProfile.h>
#include <morph/Elliptic.h>
#include <tbl/TubularProfile.h>
#include <tbl/TubularCrossSection.h>
#include <tbl/TubularMesher.h>
#include <tbl/RandomProfileRule.h>
#include <ctime>

namespace alo {
   
LeafTest::LeafTest() 
{
    std::time_t secs = std::time(0);
	Uniform<Lehmer> lmlcg(secs);
    
    m_mesh = new AdaptableMesh;

    TubularCrossSection tbcrs;
    tbcrs.create(6, .1f, .09f);

    TubularProfile prof0;
    Matrix33F frm0 = TubularProfile::calculateFrame0(Vector3F(0.f, .39f, 0.f), Vector3F(-.3f, .27f, -.17f));
    prof0.begin(Vector3F(0.f, -.1f, 0.f), frm0);

    typedef tbl::RandomProfileRule<Uniform<Lehmer> > TbRuleTyp;

    TbRuleTyp tbrule(&lmlcg);
    tbrule.create(Vector3F(0.f, .39f, 0.f), Vector3F(-.3f, .27f, -.17f));
    prof0.randomSegments<TbRuleTyp>(7, Float2(.01f, .5f), 1.f, 0.2f, tbrule);
    prof0.smooth();
    prof0.end();

    TubularMesher tbmshr;
    tbmshr.attach(m_mesh);
    tbmshr.triangulate(tbcrs, prof0);
    tbmshr.detach();

    morph::LeafProfile lpf;
    lpf._resolution.set(8, 1);
    lpf._oblique.set(.1f, .1f);
    lpf._twistBendRoll.set(.2f, -.1f, .4f);
    lpf._size.set(1.f, .7f, .3f);
    morph::Elliptic elp;
    elp.create(lpf);

    RegionMesher mshr;
    mshr.attach(m_mesh);

    mshr.triangulate<morph::Elliptic>(elp);

    mshr.detach();
 #if 0   
    BoundingBox shapeBox;
    m_mesh->getAabb(shapeBox);
    const float span = shapeBox.getLongestDistance();
    shapeBox.expand(span * .001f);
    shapeBox.round();
    std::cout<<"\n shape box"<<shapeBox;
    
    const float ssz = span * .001f;
	std::cout << "\n sample size " << ssz;
    
    typedef SurfaceGeodesicSample SampleTyp;
    typedef SampleInterp<SurfaceGeodesicSample> SampleInterpTyp;
    typedef smp::Triangle<SampleTyp > SamplerTyp;
    SamplerTyp sampler;
	sampler.setSampleSize(ssz);
    
    typedef sds::SpaceFillingVector<SampleTyp> PntArrTyp;
	PntArrTyp pnts;
    
    SampleTyp asmp;
    SampleInterpTyp interp;
    const int nt = m_mesh->numTriangles();
    for(int i=0;i<nt;++i) {
        m_mesh->getTriangle<SampleTyp, SamplerTyp >(sampler, i);
        sampler.addSamples <PntArrTyp, SampleInterpTyp, Uniform<Lehmer> >(pnts, asmp, interp, &lmlcg);
    }
    const int ns = pnts.size();
    std::cout<<"\n n samples "<<ns;

    sds::FZOrderCurve sfc;

    gdf::GeodesicDistance geod;
    geod.computeHexahedronField<PntArrTyp, sds::FZOrderCurve >(pnts, &sfc, shapeBox);

    typedef sdf::SsdfBuildRule<sds::FZOrderCurve> BuildRuleTyp;
    BuildRuleTyp rule(&sfc);
    
    typedef sdf::SsdfBuilder<SampleTyp, float, 4, 7, BuildRuleTyp > BuilderTyp;
    BuilderTyp builder;
    
    typedef sdf::SsdField FieldTyp;
    FieldTyp field;
    
    rule.setBBox(shapeBox);
    builder.build(&pnts, rule);
    builder.save<FieldTyp>(field, rule);

    ver1::H5IO hio;
    bool stat = hio.begin("./leaf.hes", HDocument::oCreate );
    
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
    
    ga.close();
    hio.end();
#endif
    DrawableResource *rec = createResource();
    setResource(rec);
}

LeafTest::~LeafTest()
{}
    
void LeafTest::update()
{
    computeMesh();
}

void LeafTest::addDrawableTo(DrawableScene *scene)
{
    setDrawableScene(scene);
    computeMesh();  
}

void LeafTest::computeMesh()
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
