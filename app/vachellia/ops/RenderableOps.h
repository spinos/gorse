/*
 *  RenderableOps.h
 *  vachellia
 *
 *  2019/4/26
 */

#ifndef RENDERABLE_GLYPH_OPS_H
#define RENDERABLE_GLYPH_OPS_H

#include <qt_graph/GlyphOps.h>
#include <interface/RenderableObject.h>
#include <grd/GridSamples.h>

namespace alo {
    
class RenderableScene;

class RenderableOps : public GlyphOps, public RenderableObject
{
    RenderableScene *m_scene;
    
public:
    RenderableOps();
    virtual ~RenderableOps();

    virtual bool hasRenderable() const override;
    virtual void setDrawableVisible(bool x) override;
    
    virtual void addRenderableTo(RenderableScene *scene) = 0;
	virtual void removeRenderableFromScene();

    virtual bool hasInstance() const = 0;
    virtual bool hasGeodesicSamples() const = 0;
    
    virtual void expandAabb(float *box) const = 0;
    virtual float mapDistance(const float *q) const = 0;
    virtual Vector3F mapNormal(const float *q) const = 0;
    virtual float mapLocalDistance(const float *q) const = 0;

	virtual void genSamples(sds::SpaceFillingVector<grd::PointSample> &samples) const = 0;
    
    virtual bool getVisibleState() const override;

protected:
    void setRenderableScene(RenderableScene *x);
    RenderableScene *renderableScene();
	
private:
    
};

}

#endif

