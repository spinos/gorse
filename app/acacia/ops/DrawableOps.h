/*
 *  DrawableOps.h
 *  aloe
 *
 *  drawable object ops
 *  id each object created
 *  hold multiple resources
 *
 */

#ifndef DRAWABLE_GLYPH_OPS_H
#define DRAWABLE_GLYPH_OPS_H

#include <qt_graph/GlyphOps.h>
#include <qt_ogl/DrawableResourceArray.h>

namespace alo {

namespace ver1 {
class ATriangleMesh;
}

class DrawableScene;
class DrawableObject;

class DrawableOps : public GlyphOps, public DrawableResourceArray
{
	DrawableScene *m_scene;
	int m_drawCounter;

public:
	DrawableOps();
	virtual ~DrawableOps();

	virtual bool hasDrawable() const override;

	virtual void addDrawableTo(DrawableScene *scene) = 0;
	virtual void removeDrawableFromScene();

protected:
	void setDrawableScene(DrawableScene *x);
	DrawableScene *drawableScene();
	DrawableObject *createDrawable();
	
	void updateMeshResouce(DrawableResource *rec, const ver1::ATriangleMesh *mesh, bool showUV=false);

private:
	
};

}

#endif
