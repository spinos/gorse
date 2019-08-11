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
#include <gdf/HexahedronDistanceBuilder.h>
#include <gdf/HexahedronDistanceBuildRule.h>
#include <sds/SparseFieldGradientLookup.h>
#include <sds/SparseFieldGradientLookupRule.h>
#include <tbl/TubularProfile.h>
#include <tbl/TubularCrossSection.h>
#include <tbl/TubularMesher.h>
#include <tbl/RandomProfileRule.h>
#include <ctime>

namespace alo {
   
BranchTest::BranchTest() 
{
    std::time_t secs = std::time(0);
	Uniform<Lehmer> lmlcg(secs);

    typedef tbl::RandomProfileRule<Uniform<Lehmer> > TbRuleTyp;

    TbRuleTyp tbrule(&lmlcg);
    tbrule.setFirstDirection(Vector3F(.1f, 3.9f, -.1f));
    tbrule.setMajorDirection(Vector3F(-3.3f, 2.7f, -.7f));
    
    m_mesh = new AdaptableMesh;
    //createRandomRibbon<Uniform<Lehmer> >(*m_mesh, 7, 101, &lmlcg);
    
    TubularCrossSection tbcrs;
    tbcrs.create(32, .61f, .59f);
    
    TubularProfile prof0;
    prof0.begin(Vector3F(0.f, -.33f, 0.f));
#if 0
    prof0<<Vector3F(0.01f, 2.99f, -0.11f);
    prof0<<Vector3F(1.06f, 1.79f, 0.2f);
    prof0<<Vector3F(1.29f, 1.29f, 1.75f);
    prof0<<Vector3F(-.3f, .85f, 1.97f);
    prof0<<Vector3F(-.42f, -.5f, 1.77f);
    prof0<<Vector3F(.71f, -.2f, 1.67f);
    prof0<<Vector3F(1.25f, -.3f, 1.57f);
#else
    prof0.randomSegments<TbRuleTyp>(9, Float2(1.4f, 1.5f), 1.f, 0.2f, tbrule);
#endif
    prof0.smooth();
    prof0.end();
    
    TubularMesher tbmshr;
    tbmshr.attach(m_mesh);
    tbmshr.triangulate(tbcrs, prof0);

    Vector3F ori[3];
    ori[0] = prof0.interpolatePosition(.3f);
    ori[1] = prof0.interpolatePosition(.5f);
    ori[2] = prof0.interpolatePosition(.7f);

    Vector3F vf[3];
    vf[0] = Vector3F(.3f, 2.9f, .5f);
    vf[1] = Vector3F(.4f, 2.9f, -.4f);
    vf[2] = Vector3F(-.4f, 2.9f, -.3f);

    Vector3F vm[3];
    vm[0] = Vector3F(1.7f, 2.2f, -1.9f);
    vm[1] = Vector3F(1.f, 2.1f, 1.1f);
    vm[2] = Vector3F(-1.9f, 2.3f, -1.f);

    int nss[3] = {7, 6, 5};

    for(int i=0;i<3;++i) {
        prof0.begin(ori[i]);

        tbrule.setFirstDirection(vf[i]);
        tbrule.setMajorDirection(vm[i]);

        prof0.randomSegments<TbRuleTyp>(nss[i], Float2(1.4f, 1.5f), 1.f, 0.2f, tbrule);
        prof0.smooth();
        prof0.end();

        tbmshr.triangulate(tbcrs, prof0);

        ori[i] = prof0.interpolatePosition(.3f);
    }

    for(int i=0;i<3;++i) {
        prof0.begin(ori[i]);

        int j = (i+1) % 3;

        tbrule.setFirstDirection(vf[j]);
        tbrule.setMajorDirection(vm[j]);

        prof0.randomSegments<TbRuleTyp>(5, Float2(1.f, 1.f), 1.f, 0.2f, tbrule);
        prof0.smooth();
        prof0.end();

        tbmshr.triangulate(tbcrs, prof0);

    }

    tbmshr.detach();
    
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
    typedef gdf::HexahedronDistanceBuildRule<sds::FZOrderCurve> FieldRuleTyp;
    FieldRuleTyp rule(&sfc);
    rule.setP(6);
    rule.setBBox(shapeBox);
    
    typedef sds::SparseCubicField<float> FieldTyp;
    FieldTyp field;
    gdf::HexahedronDistanceBuilder<FieldTyp> builder;

    builder.attach<FieldRuleTyp>(&field, rule);
    builder.measure<PntArrTyp, FieldRuleTyp>(pnts, rule);    
    builder.detach<FieldRuleTyp>(rule);

    typedef sds::SparseFieldGradientLookupRule<sds::FZOrderCurve> LookupRuleTyp;
    LookupRuleTyp lookupRule(&sfc);
    lookupRule.setP(6);
    lookupRule.setBBox(shapeBox);

    sds::SparseFieldGradientLookup<FieldTyp> reader;
    reader.attach(&field);

    Vector3F sum(0,0,0);
    int nupd = 0;

    for(int i=0;i<ns;++i) {
        const float *p = (const float *)&pnts[i]._pos;
        reader.lookupGradient<LookupRuleTyp>(p, lookupRule);
        if(reader.isStatusUnknown()) continue;

        if(reader.isStatusUpdate()) nupd++;

        pnts[i]._geod = reader.interpolate(p);
        reader.interpolateGradient((float *)&pnts[i]._grad, p);

        sum += pnts[i]._grad;
    }

    sum *= 1.f / ns;

    std::cout << "\n avg grad " << sum
                << "\n n update " << nupd;

    reader.detach();
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
