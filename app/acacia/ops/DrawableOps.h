/*
 *  DrawableOps.h
 *  aloe
 *
 *  resource[]
 *  |
 *  object ---- scene
 *  id each drawable object created
 *  hold multiple resources
 *  send associated object to scene
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
class VisibilityState;

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
	
	const int &frameNumber() const;

	static void UpdateMeshResouce(DrawableResource *rec, const ver1::ATriangleMesh *mesh, bool showUV=false);
	void initiateResource(DrawableResource *rec);
/// relocate or edit associated drawable object
	void processResource(DrawableResource *rec, bool forcedToRelocate=false);
	void processResourceNoLock(DrawableResource *rec);
	void processResource(DrawableResource *rec, const VisibilityState &vis);
/// n resouce remove redundant ones
	void setDrawableSize(int n);
	void lockScene();
	void unlockScene();

private:
	
};

}

#endif
