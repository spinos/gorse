#ifndef RENDERABLE_GLYPH_OPS_H
#define RENDERABLE_GLYPH_OPS_H

#include <qt_graph/GlyphOps.h>
#include <interface/RenderableObject.h>

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
    
protected:
    void setRenderableScene(RenderableScene *x);
	
private:
    
};

}

#endif

