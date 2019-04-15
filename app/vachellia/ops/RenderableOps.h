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
    
protected:
    
private:
    
};

}

#endif

