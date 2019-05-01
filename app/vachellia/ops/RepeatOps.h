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

struct TestCell;

template<typename T, typename Tc>
class WorldGrid;

template<typename T, typename Tc, typename Tf>
class WorldGridBuildRule;

template<typename T, typename Tc>
class WorldGridBuilder;

template<typename T>
class WorldGridLookupRule;

}

namespace sds {
struct FZOrderCurve;
}

class RepeatOps : public TransformOps {

	ElementVector<RenderableOps> m_inOps;
    grd::IndexGrid *m_grid;

    typedef grd::WorldGrid<int, grd::TestCell > WorldTyp;
    WorldTyp *m_worldGrid;

    typedef grd::WorldGridBuildRule<int, grd::TestCell, sds::FZOrderCurve > WorldRuleTyp;
    WorldRuleTyp *m_worldRule;

    typedef grd::WorldGridBuilder<int, grd::TestCell > WorldBuilderTyp;
    WorldBuilderTyp *m_worldBuilder;

    typedef grd::WorldGridLookupRule<WorldTyp > WorldLookupRuleTyp;
    WorldLookupRuleTyp *m_worldLookupRule;

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

