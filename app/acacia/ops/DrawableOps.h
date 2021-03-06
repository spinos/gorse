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
#include <math/Hexahedron.h>

namespace alo {

namespace ver1 {
class ATriangleMesh;
}

class DrawableScene;
class DrawableObject;
class VisibilityState;
class BoundingBox;

class DrawableOps : public GlyphOps, public DrawableResourceArray
{
	Hexahedron m_bound;
	DrawableScene *m_scene;
	int m_drawCounter;
	bool m_drawableVisible;

public:
	DrawableOps();
	virtual ~DrawableOps();

	virtual bool hasDrawable() const override;
	virtual void setDrawableVisible(bool x) override;
	virtual void getDrawableBound(Hexahedron &b) const override;

	virtual void addDrawableTo(DrawableScene *scene) = 0;
	virtual void removeDrawableFromScene();

protected:
	void setDrawableScene(DrawableScene *x);
	DrawableScene *drawableScene();
	DrawableObject *createDrawable();
	
	const int &frameNumber() const;
	const bool &drawableVisible() const;

	static void UpdateMeshResouce(DrawableResource *rec, const ver1::ATriangleMesh *mesh, bool showUV=false);

	void processResourceNoLock(DrawableResource *rec);
	void processResource(DrawableResource *rec, const VisibilityState &vis);

	void setDrawableSize(int n);
	void lockScene();
	void unlockScene();

	void setBound(const Hexahedron &x);
	void setBound(const BoundingBox &x);
	void setBound(const ver1::ATriangleMesh *mesh);

private:
	
};

}

#endif
