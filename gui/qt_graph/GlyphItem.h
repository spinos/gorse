/*
 *  GlyphItem.h
 *
 *  top level item holds instance of attribs
 *  type, id, and ops
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
struct Connectable;
class VisibilityControlItem;

class GlyphItem : public QGraphicsPathItem
{
	int m_glyphType;
	int m_glyphId;
	
public:
	enum { Type = UserType + 1 };
	
	GlyphItem(const QPixmap & iconPix, int gtyp,
			QGraphicsItem * parent = 0 );
	virtual ~GlyphItem();

	void setGlyphId(int x);
							
	void moveBlockBy(const QPointF & dp);
	
	int type() const { return Type; }
	
	const int &glyphType() const;
	const int &glyphId() const;
	void setOps(GlyphOps *ops);
	void setHalo(GlyphHalo* hal);
	void showHalo();
	void hideHalo();
	void activateHalo();
	void deactivateHalo();
	GlyphHalo *halo();
	GlyphOps *getOps() const;
	GlyphOps *ops();
	
	QPointF localCenter() const;
	const std::string& glyphName() const;

/// before (dis)connect with another via port	
	void postConnection(GlyphItem* another, GlyphPort* viaPort);
	void preDisconnection(GlyphItem* another, GlyphPort* viaPort);
/// after connection via port is changed
	void postDisconnection(GlyphPort* viaPort);
	void postSelection();

	void addVisibilityControl();

	void beginEditState(QGraphicsItem *item);
	void endEditState(QGraphicsItem *item);

protected:
	GlyphPort *addPort(const QString & name, 
							bool isOutgoing);
	void resizeBlock(int bx, int by);
	virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );
	virtual void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );
	virtual void mouseDoubleClickEvent( QGraphicsSceneMouseEvent * event );
	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent * event);
	
private:
	void movePort(GlyphPort *pt, const Connectable *c);
	void processContextMenu(int k);
	
private:
	QGraphicsPixmapItem *m_icon;
	GlyphOps *m_ops;
	GlyphHalo *m_halo;
	VisibilityControlItem *m_visibility;
	int m_blockWidth, m_blockHeight;

};

}
#endif