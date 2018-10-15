/*
 *  GlyphHalo.h
 *
 *  selection representation
 *
 *  Created by jian zhang on 8/4/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_GLYPH_HALO_H
#define ALO_GLYPH_HALO_H

#include <QGraphicsEllipseItem>

namespace alo {

class GlyphHalo : public QGraphicsEllipseItem
{

public:
	enum { Type = UserType + 4 };
	
	GlyphHalo(QGraphicsItem * parent = 0 );
	virtual ~GlyphHalo();
	
	int type() const { return Type; }
	
protected:
	
private:
	
};

}
#endif