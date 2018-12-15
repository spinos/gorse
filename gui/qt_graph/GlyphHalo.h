/*
 *  GlyphHalo.h
 *  aloe
 *
 *  selection representation
 *
 */

#ifndef ALO_GLYPH_HALO_H
#define ALO_GLYPH_HALO_H

#include <QGraphicsEllipseItem>

namespace alo {

class GlyphHalo : public QGraphicsEllipseItem
{
	QGraphicsEllipseItem *m_highlight;

public:
	enum { Type = UserType + 4 };
	
	GlyphHalo(QGraphicsItem * parent = 0 );
	virtual ~GlyphHalo();
	
	int type() const { return Type; }

	void setHightVisible(bool x);
	
protected:
	
private:
	
};

}
#endif