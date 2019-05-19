/*
 *  SceneGraph.h
 *  
 *
 *  Created by jian zhang on 3/30/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_SCENE_GRAPH_H
#define ALO_SCENE_GRAPH_H

#include <QGraphicsView>

QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QGraphicsItem;
class QPixmap;
class QPoint;
QT_END_NAMESPACE

namespace alo {

class GlyphScene;
class GlyphItem;
class GlyphConnection;

class SceneGraph : public QGraphicsView
{
    Q_OBJECT

public:
    SceneGraph(GlyphScene * scene, QWidget * parent = 0);
	
protected:
	bool event(QEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
	void resizeEvent (QResizeEvent * event) override;
	void keyPressEvent(QKeyEvent *event) override;

	GlyphScene * asGlyphScene();
	
signals:

private:
    void processSelect(const QPoint& pos);
	void processRemove(const QPoint& pos);
	void beginProcessView(QMouseEvent *event);
	void beginProcessItem(QMouseEvent *event);
	void endProcessItem(QGraphicsItem *item);
	void processItem(QMouseEvent *event);
	void panView(QMouseEvent *event);
	void doMoveItem(const QPoint& mousePos);
	void doMoveConnection(const QPoint& mousePos);
	void doConnectItem(QGraphicsItem* item);
	void doRemoveConnection(QGraphicsItem* item);
    void modifyDv(QPointF &dv, const GlyphItem *focused) const;
	
	enum Mode {
		mNone = 0,
		mPanView,
		mMoveItem,
		mEditState,
		mConnectItems,
		mRemoveConnection
	}; 
	
	Mode m_mode;
	GlyphConnection *m_selectedConnection;
	QPoint m_lastMosePos;
/// changed after pan
	QPoint m_sceneOrigin;
	
};

}

#endif