/*
 *  RepeatOps.h
 *  vachellia
 *
 */

#ifndef VCHL_REPEAT_OPS_H
#define VCHL_REPEAT_OPS_H

#include "TransformOps.h"
#include <math/ElementVector.h>
#include <sdb/Types.h>

namespace alo {

namespace grd {
class IndexGrid;

template<typename T>
struct TestCell;

template<typename T, typename Tc>
class WorldGrid;

template<typename T, typename Tc, typename Tf>
class WorldGridBuildRule;

template<typename T, typename Tc>
class WorldGridBuilder;

template<typename T, typename Tc>
class WorldGridLookupRule;

struct TestBox;

}

namespace sds {
struct FZOrderCurve;
}

class RepeatOps : public TransformOps {

	ElementVector<RenderableOps> m_inOps;
    grd::IndexGrid *m_grid;

    typedef grd::TestCell<grd::IndexGrid> WorldCellTyp;

    typedef grd::WorldGrid<int, WorldCellTyp > WorldTyp;
    WorldTyp *m_worldGrid;

    typedef grd::WorldGridBuildRule<int, WorldCellTyp, sds::FZOrderCurve > WorldRuleTyp;
    WorldRuleTyp *m_worldRule;

    typedef grd::WorldGridBuilder<int, WorldCellTyp > WorldBuilderTyp;
    WorldBuilderTyp *m_worldBuilder;

    typedef grd::WorldGridLookupRule<WorldTyp, WorldCellTyp > WorldLookupRuleTyp;
    WorldLookupRuleTyp *m_worldLookupRule;

    typedef grd::TestBox ObjectTyp;
    SimpleBuffer<ObjectTyp> m_objs;


public:
	enum { Type = 800002 };

    RepeatOps();
    virtual ~RepeatOps();

    virtual std::string opsName() const override;

    virtual void addRenderableTo(RenderableScene *scene) override;

    virtual void update() override;
    
    virtual bool intersectRay(const Ray& aray, IntersectResult& result) override;

    virtual bool canConnectTo(GlyphOps *another, const std::string &portName) const override;
    virtual void connectTo(GlyphOps *another, const std::string &portName) override;
    virtual void disconnectFrom(GlyphOps *another, const std::string &portName) override;

    virtual float mapDistance(const float *q) const override;
    virtual Vector3F mapNormal(const float *q) const override;
    
protected:
    
private:
    
};

}

#endif

