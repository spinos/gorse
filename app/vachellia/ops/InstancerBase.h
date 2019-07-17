/*
 *  InstancerBase.h
 *  vachellia
 *
 *  scattering input ops
 *
 *  2019/7/18
 */

#ifndef VCHL_INSTANCER_BASE_H
#define VCHL_INSTANCER_BASE_H

#include "RenderableOps.h"
#include <math/ElementVector.h>
#include <sdb/Types.h>

namespace alo {

namespace grd {

template<typename T>
class LocalGrid;

template<typename T>
struct GridInCell;

template<typename T, typename Tc>
class WorldGrid;

template<typename T, typename Tf>
class WorldGridBuildRule;

template<typename T, typename Tc>
class WorldGridBuilder;

template<typename T, typename Tc, typename Ti>
class WorldGridLookupRule;

template<typename T1, typename T2>
class ObjectInstancer;

struct TestInstance;

template<typename T, typename Tp>
class LocalGridLookupRule;

class InstanceRecord;

}

namespace sds {
struct FZOrderCurve;
}

class InstancerBase {

	ElementVector<RenderableOps> m_inOps;

    typedef grd::GridInCell<grd::LocalGrid<float> > WorldCellTyp;

    typedef grd::WorldGrid<int, WorldCellTyp > WorldTyp;
    WorldTyp *m_worldGrid;

    typedef grd::WorldGridBuildRule<int, sds::FZOrderCurve > WorldRuleTyp;
    WorldRuleTyp *m_worldRule;

    typedef grd::WorldGridBuilder<int, WorldCellTyp > WorldBuilderTyp;
    WorldBuilderTyp *m_worldBuilder;

    typedef grd::ObjectInstancer<grd::TestInstance, RenderableOps > InstancerTyp;
    InstancerTyp *m_instancer;

    typedef grd::LocalGridLookupRule<grd::LocalGrid<float>, InstancerTyp > LocalLookupRuleTyp;

    typedef grd::WorldGridLookupRule<WorldTyp, WorldCellTyp, LocalLookupRuleTyp > WorldLookupRuleTyp;
    WorldLookupRuleTyp *m_worldLookupRule;

    bool m_isActive;

public:

    InstancerBase();
    virtual ~InstancerBase();

protected:

    void appendInputRenderable(RenderableOps *r);
    void removeInputRenderable(RenderableOps *r);
    bool hasInputRenderable(RenderableOps *r) const;
    bool isInputRenderableDirty() const;
    bool isLookupEmpty() const;
    template<typename T>
    bool traceRay(T &result, const float *rayData) const;

    void setInstancerActivated(RenderableScene *scene, bool x);
    
    void updateInstancer(RenderableScene *scene, bool isAppending);
    void setLookupMaxNumSteps(int x);
    void loadInstanceRecord(grd::InstanceRecord &rec);

    bool isInstancerActivated() const;
    const BoundingBox &instancerAabb() const;
    int numInputRenderables() const;
    const int &numInstancedObjects() const;
    const RenderableOps *inputRenderable(int i) const;
    
private:
    void updateInstancerInProgress(bool isAppending);
    
};

template<typename T>
bool InstancerBase::traceRay(T &result, const float *rayData) const
{
    return m_worldLookupRule->lookup(result, rayData);
}

}

#endif

