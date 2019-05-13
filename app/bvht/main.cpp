/*
 *  World/Local grid with bvh test
 *  2019/5/8
 */

#include <cstring>
#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
#include <grd/LocalGrid.h>
#include <grd/LocalGridBuilder.h>
#include <grd/LocalGridBuildRule.h>
#include <grd/LocalGridLookupRule.h>
#include <grd/WorldGridBuilder.h>
#include <grd/WorldGridBuildRule.h>
#include <grd/WorldGridLookupRule.h>
#include <grd/TestCell.h>
#include <grd/GridInCell.h>
#include <sds/FZOrder.h>
#include <ctime>

using namespace alo;

struct BoxObject {

	BoundingBox _bbox;

	const BoundingBox &aabb() const 
	{ return _bbox; }

	void getAabb(float *b) const
	{ memcpy(b, _bbox.data(), 24); }

	void expandAabb(float *b) const
	{
		b[0] -= .5f;
		b[1] -= .5f;
		b[2] -= .5f;
		b[3] += .5f;
		b[4] += .5f;
		b[5] += .5f;
	}
    
    float mapDistance(const float *p) const 
    {
        if(isPointOutsideBox(p, _bbox.data()))
            return distanceOutsidePointToBox(p, _bbox.data());

        return -distanceInsidePointToBox(p, _bbox.data());
    }

    void mapNormal(float *nml, const float *p) const 
    {
    	normalOnBox(nml, p, _bbox.data());
    }

    void limitStepSize(float &x) const
    {
    	//if(x > 1e-3f && x < .1f) x = .1f;
		//if(x > 2.f) x = 2.f;
    }
/// cover the faces
    template<typename T>
	void genSamples(sds::SpaceFillingVector<T> &samples) const
	{
	    const float *b = _bbox.data();
        const float dx = b[3] - b[0];
        const float dy = b[4] - b[1];
        const float dz = b[5] - b[2];
        const float xArea = dy * dz;
        const float yArea = dx * dz;
        const float totalArea = (b[4] - b[1]) * (b[3] - b[0]) + xArea + yArea;
        const float xRatio = xArea / totalArea;
        const float zRatio = (xArea + yArea) / totalArea;
		T ap;
		for(int i=0;i<4000;++i) {
			randomPointOnAabb((float *)&ap._pos, b, dx, dy, dz, xRatio, zRatio, .05f);
			samples << ap;
		}
	}

};

void testBvh()
{
    BoxObject *bo = new BoxObject;
    bo->_bbox.setMin(-3.f, 0.f, -4.f);
    bo->_bbox.setMax( 3.f, 25.f,  4.f);
	
	typedef grd::ObjectInstancer<grd::TestInstance, BoxObject> InstancerTyp;
    InstancerTyp *m_instancer = new InstancerTyp;
	
    m_instancer->addObject(bo);
	
	static const int udim = 100;
    static const int vdim = 100;
    static const float spacing = 23.9;
    static const float xzSpan = 2.5f;
    static const float ySpan = 0.1f;
    static const float coverOrigin = 0;
    static const float scaleSpan = .5f;
    
    std::time_t secs = std::time(0);
    m_instancer->createPhalanx(udim, vdim, spacing, xzSpan, ySpan, coverOrigin, scaleSpan, secs);
	
	sds::FZOrderCurve sfc;
    sfc.setCoord(32.f, 32.f, 32.f, 16.f);
    
    typedef grd::LocalGridBuildRule<sds::FZOrderCurve> CellBuildRuleTyp;
    CellBuildRuleTyp cellRule(&sfc);
    cellRule.setP(5);
	
	typedef grd::LocalGridBuilder<grd::LocalGrid<float> > CellBuilderTyp;
    CellBuilderTyp cellBuilder;
	
	typedef grd::GridInCell<grd::LocalGrid<float> > WorldCellTyp;

    typedef grd::WorldGrid<int, WorldCellTyp > WorldTyp;
    WorldTyp *m_worldGrid = new WorldTyp;
    
    typedef grd::WorldGridBuildRule<int, sds::FZOrderCurve > WorldRuleTyp;
    WorldRuleTyp *m_worldRule = new WorldRuleTyp;
    const int cencz[4] = {0,0,0,256};
    m_worldRule->setCenterCellSize(cencz);

    typedef grd::WorldGridBuilder<int, WorldCellTyp > WorldBuilderTyp;
    WorldBuilderTyp *m_worldBuilder = new WorldBuilderTyp;

    m_worldBuilder->attach(m_worldGrid);

    m_worldBuilder->addInstances<InstancerTyp, WorldRuleTyp, CellBuilderTyp, CellBuildRuleTyp >(*m_instancer, *m_worldRule, cellBuilder, cellRule);
   
    m_worldBuilder->detach();
    
    typedef grd::LocalGridLookupRule<grd::LocalGrid<float>, InstancerTyp > LocalLookupRuleTyp;
    
    typedef grd::WorldGridLookupRule<WorldTyp, WorldCellTyp, LocalLookupRuleTyp > WorldLookupRuleTyp;
    WorldLookupRuleTyp *m_worldLookupRule = new WorldLookupRuleTyp;
    m_worldLookupRule->attach(m_worldGrid);
    m_worldLookupRule->setPrimitiveRule<InstancerTyp>(m_instancer);
    
}

int main(int argc, char *argv[])
{
    std::cout<<"\n test bvh ";
    testBvh();
    std::cout<<"\n passed test ";
      
    return 0;
}
