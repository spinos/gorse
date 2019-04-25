/*
 *  RepeatOps.h
 *  vachellia
 *
 */

#ifndef VCHL_REPEAT_OPS_H
#define VCHL_REPEAT_OPS_H

#include "TransformOps.h"
#include <math/ElementVector.h>

namespace alo {
    
class IndexGrid;

class RepeatOps : public TransformOps {

	ElementVector<RenderableOps> m_inOps;
    IndexGrid *m_grid;

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

