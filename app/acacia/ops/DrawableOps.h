/*
 *  DrawableOps.h
 *
 *
 */

#ifndef DRAWABLE_GLYPH_OPS_H
#define DRAWABLE_GLYPH_OPS_H

#include <qt_graph/GlyphOps.h>

namespace alo {

class DrawableScene;
class DrawableObject;

class DrawableOps : public GlyphOps
{
	DrawableScene *m_scene;
	DrawableObject *m_drawable;
	
public:
	DrawableOps();
	virtual ~DrawableOps();

	virtual bool hasDrawable() const override;

	virtual void addDrawableTo(DrawableScene *scene) = 0;
	virtual void removeDrawableFromScene();

protected:
	void setDrawable(DrawableObject *x);
	DrawableObject *drawable();
	void setDrawableScene(DrawableScene *x);
	DrawableScene *drawableScene();

private:
	
};

}

#endif
