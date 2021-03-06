/*
 *  BoxOps.h
 *  vachellia
 *
 *  2019/4/26
 */

#ifndef VCHL_BOX_OPS_H
#define VCHL_BOX_OPS_H

#include "TransformOps.h"
#include <math/ElementVector.h>

namespace alo {

class BoxOps : public TransformOps {
    
    float m_boxSize[3];
    ElementVector<GlyphConnection> m_outOps;

public:
	enum { Type = 299100 };

    BoxOps();
    virtual ~BoxOps();

    virtual std::string opsName() const override;

    virtual void addRenderableTo(RenderableScene *scene) override;

    virtual bool hasInstance() const override;
    virtual bool hasGeodesicSamples() const override;
    virtual bool hasSurfaceSamples() const override;
    
    virtual void update() override;
    
    virtual void connectTo(GlyphOps *another, const std::string &portName, GlyphConnection *line) override;
    virtual void disconnectFrom(GlyphOps *another, const std::string &portName, GlyphConnection *line) override;
    virtual QString getShortDescription() const override;

protected:
    
private:
    
};

}

#endif

