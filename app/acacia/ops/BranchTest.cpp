/*
 *  BranchTest.cpp
 *  acacia
 *
 */

#include "BranchTest.h"
#include <qt_ogl/DrawableScene.h>
#include <qt_ogl/DrawableObject.h>
#include <qt_ogl/DrawableResource.h>
#include <sds/SpaceFillingVector.h>
#include <sds/FZOrder.h>
#include <sds/SparseFieldBuilder.h>
#include <sds/SparseFieldBuildRule.h>
#include <geom/AdaptableMesh.h>
#include <mesh/randomRibbon.h>
#include <rng/Lehmer.h>
#include <rng/Uniform.h>
#include <smp/Triangle.h>
#include <smp/SurfaceGeodesicSample.h>
#include <sds/SparseCubicField.h>
#include <gdf/GeodesicDistance.h>
#include <tbl/TubularProfile.h>
#include <tbl/TubularCrossSection.h>
#include <tbl/TubularMesher.h>
#include <tbl/RandomProfileRule.h>
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
#include <h5_smp/HSurfaceGeodesicSample.h>
#include <morph/Monopodial.h>
#include <morph/PlantMesher.h>
#include <ctime>

namespace alo {
   
BranchTest::BranchTest() 
{
    std::time_t secs = std::time(0);
	Uniform<Lehmer> lmlcg(secs);

    morph::Monopodial<Uniform<Lehmer> > plantRule(&lmlcg);
    morph::Plant aplant;
    morph::PlantProfile plantF;
    plantF.setGrowDirections(Vector3F(0.f, 1.f, 0.f), Vector3F(-.1f, 1.3f, .05f));
    plantF.setAge(11);
    plantF.setMinBranchSeason(3);
    morph::StemProfile stemF;
    stemF.setAxilAngle(.7f);
    plantRule.grow(&aplant, plantF, stemF);

    m_mesh = new AdaptableMesh;

    morph::PlantMesher plmshr;
    plmshr.attach(m_mesh);
    plmshr.triangulate(aplant, plantF);
    plmshr.detach();

#if 1
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
    bool stat = hio.begin("./trunk.hes", HDocument::oCreate );
    
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
    
    grd::LocalGridBuilder<grd::LocalGrid<float> >::ObjectInCellMap cellMap;
    locBuilder.measure<OutSampleTyp, LocBuildTyp>(cellMap, orignalSamples, locRule);

    locBuilder.detach(&cellMap);

    HLocalGrid hlocG("/asset/localGrid");
    hlocG.save<grd::LocalGrid<float> >(locG);
    hlocG.close();
    
    PntArrTyp outpnts = pnts.reduceTo<Uniform<Lehmer> >(10000, &lmlcg);

    HSurfaceGeodesicSample hgeod("/asset/geod");
    hgeod.save<PntArrTyp>(outpnts);
    hgeod.close();
    
    ga.close();
    hio.end();
#endif
    DrawableResource *rec = createResource();
    setResource(rec);
}

BranchTest::~BranchTest()
{}
    
void BranchTest::update()
{
    computeMesh();
}

void BranchTest::addDrawableTo(DrawableScene *scene)
{
    setDrawableScene(scene);
    computeMesh();  
}

void BranchTest::computeMesh()
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
