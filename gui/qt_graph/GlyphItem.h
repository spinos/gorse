/*
 *  GlyphItem.h
 *  gorse
 *
 *  top level item holds instance of ops
 *
 *  2019/5/25
 */

#ifndef GAR_GLYPH_ITEM_H
#define GAR_GLYPH_ITEM_H

#include <QGraphicsPathItem>
#include <vector>

QT_BEGIN_NAMESPACE
class QPixmap;
class QGraphicsPixmapItem;
class QGraphicsSimpleTextItem;
class QPolygonF;
QT_END_NAMESPACE

namespace alo {

class GlyphPort;
class GlyphHalo;
class GlyphOps;
struct Connectable;
class ActivationControlItem;
class VisibilityControlItem;
class GlyphConnection;

class GlyphItem : public QGraphicsPathItem
{
	int m_glyphType;
	int m_glyphId;
	int m_blockWidth;
	int m_blockHeight;
	bool m_postLoadVisible;
	bool m_postLoadActivated;
	
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
	const QPixmap &iconPix() const;
	QPolygonF getCollisionPolygon(const QPointF &dv) const;
	QPointF getCollisionCenter() const;
/// by name
	GlyphPort *findPort(const std::string &name) const;
	
	QPointF localCenter() const;
	std::string glyphName() const;

    bool canConnectTo(GlyphItem* another, GlyphPort* viaPort);
    void preConnection(GlyphItem* another, GlyphPort* viaPort);
	void postConnection(GlyphItem* another, GlyphPort* viaPort, GlyphConnection *conn);
	void preDisconnection(GlyphItem* another, GlyphPort* viaPort, GlyphConnection *conn);
	void onInputChange(GlyphItem* another, GlyphPort* viaPort);
	void postDisconnection(GlyphPort* viaPort);
	void postSelection();

    void addEnableControl();
	void addVisibilityControl();

	void beginEditState(QGraphicsItem *item);
	void endEditState(QGraphicsItem *item);
    
    void getConnections(std::vector<GlyphConnection *> &conns,
            bool incomingOnly);

    void genToolTip();
    void updateOps();
    void setPostLoadVisibleState(const bool &x);
    void setPostLoadActivatedState(const bool &x);
    void postLoad();

protected:
	GlyphPort *addPort(const QString & name, 
							bool isOutgoing);
	void resizeBlock(int bx, int by);
	virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );
	virtual void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );
	virtual void mouseDoubleClickEvent( QGraphicsSceneMouseEvent * event );
	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent * event);
	virtual void makeCollisionPolygon(QPolygonF &poly, const QPointF &dv) const;

private:
	void movePort(GlyphPort *pt, const Connectable *c);
	void processContextMenu(int k);
	void processFileContextMenu(int k);

private:
	QPixmap *m_iconPix;
	QGraphicsPixmapItem *m_icon;
	GlyphOps *m_ops;
	GlyphHalo *m_halo;
    ActivationControlItem *m_activation;
	VisibilityControlItem *m_visibility;
	QGraphicsSimpleTextItem *m_opsLabel;

};

}
#endif