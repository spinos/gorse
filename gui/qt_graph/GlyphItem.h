/*
 *  GlyphItem.h
 *
 *  top level item holds instance of attribs
 *
 *  Created by jian zhang on 3/31/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef GAR_GLYPH_ITEM_H
#define GAR_GLYPH_ITEM_H

#include <QGraphicsPathItem>

QT_BEGIN_NAMESPACE
class QPixmap;
class QGraphicsPixmapItem;
QT_END_NAMESPACE

namespace alo {

class GlyphPort;
class GlyphHalo;
class GlyphOps;

class GlyphItem : public QGraphicsPathItem
{
	int m_glyphType;
	
public:
	enum { Type = UserType + 1 };
	
	GlyphItem(const QPixmap & iconPix, int gtyp,
			QGraphicsItem * parent = 0 );
	
	GlyphPort * addPort(const QString & name, 
							bool isOutgoing);
							
	void finalizeShape();
	void moveBlockBy(const QPointF & dp);
	
	int type() const { return Type; }
	
	void setGlyphType(int x);
	const int & glyphType() const;
	
	void setOps(GlyphOps *ops);
	void setHalo(GlyphHalo* hal);
	void showHalo();
	void hideHalo();
	GlyphHalo* halo();
	
	QPointF localCenter() const;
	const std::string& glyphName() const;

/// before (dis)connect with another via port	
	void postConnection(GlyphItem* another, GlyphPort* viaPort);
	void preDisconnection(GlyphItem* another, GlyphPort* viaPort);
/// after connection via port is changed
	void postDisconnection(GlyphPort* viaPort);
	void postSelection();
	
protected:
	void resizeBlock(int bx, int by);
	virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );
	virtual void mouseDoubleClickEvent( QGraphicsSceneMouseEvent * event );
	
private:
	void centerIcon();
	void movePorts(int n, bool downSide);
	
private:
	QGraphicsPixmapItem * m_icon;
	GlyphOps *m_ops;
	GlyphHalo* m_halo;
	int m_blockWidth, m_blockHeight;
	
};

}
#endif