/*
 *  DrawableOps.h
 *  aloe
 *
 *  drawable object ops
 *  id each object created
 *  hold multiple objects and resources
 *
 */

#ifndef DRAWABLE_GLYPH_OPS_H
#define DRAWABLE_GLYPH_OPS_H

#include <qt_graph/GlyphOps.h>
#include <vector>

namespace alo {

namespace ver1 {
class ATriangleMesh;
}

class DrawableScene;
class DrawableObject;
class DrawableResource;

class DrawableOps : public GlyphOps
{
	std::vector<DrawableObject *> m_drawables;
	std::vector<DrawableResource *> m_resources;
	DrawableScene *m_scene;
	int m_drawCounter;

public:
	DrawableOps();
	virtual ~DrawableOps();

	virtual bool hasDrawable() const override;

	virtual void addDrawableTo(DrawableScene *scene) = 0;
	virtual void removeDrawableFromScene();

protected:
	void setDrawable(DrawableObject *x, int i=0);
	DrawableObject *drawable(int i=0);
	void setDrawableScene(DrawableScene *x);
	DrawableScene *drawableScene();
	DrawableObject *createDrawable();
	DrawableResource *createResource();
	void setResource(DrawableResource *x, int i=0);
	DrawableResource *resource(int i=0);
	void updateMeshResouce(DrawableResource *rec, const ver1::ATriangleMesh *mesh, bool showUV=false);
	DrawableObject *setMeshDrawable(const ver1::ATriangleMesh *mesh, const DrawableResource *rec);
    void updateDrawableResource(DrawableObject *d, const DrawableResource *rec, int n);

private:
	
};

}

#endif
