/*
 *  CanopyOps.h
 *  vachellia
 *
 *  2019/7/18
 */

#ifndef VCHL_CANOPY_OPS_H
#define VCHL_CANOPY_OPS_H

#include "TransformOps.h"
#include "InstancerBase.h"

namespace alo {

class CanopyOps : public TransformOps, public InstancerBase {

public:
	enum { Type = 800003 };

    CanopyOps();
    virtual ~CanopyOps();

    virtual std::string opsName() const override;
    virtual void addRenderableTo(RenderableScene *scene) override;
    virtual void update() override;
    virtual bool hasInstance() const override;
    virtual bool hasEnable() const override;
    virtual bool getActivatedState() const override;
    virtual void setActivated(bool x) override;
    virtual bool intersectRay(IntersectResult& result) const override;
    virtual bool canConnectTo(GlyphOps *another, const std::string &portName) const override;
    virtual void connectTo(GlyphOps *another, const std::string &portName, GlyphConnection *line) override;
    virtual void disconnectFrom(GlyphOps *another, const std::string &portName, GlyphConnection *line) override;
    virtual QString getShortDescription() const override;

protected:
    
private:
    
};

}

#endif

