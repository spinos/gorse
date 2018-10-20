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

class DrawableOps : public GlyphOps
{

public:
	DrawableOps();
	virtual ~DrawableOps();

	virtual bool hasDrawable() const override;

	virtual void addDrawableTo(DrawableScene *scene);

protected:

private:
	
};

}

#endif
