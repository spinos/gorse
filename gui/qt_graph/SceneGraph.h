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

class SceneGraph : public QGraphicsView
{
    Q_OBJECT

public:
    SceneGraph(GlyphScene * scene, QWidget * parent = 0);
	
protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
	virtual void resizeEvent ( QResizeEvent * event );

	GlyphScene * asGlyphScene();
	
signals:
	
private:
    void processSelect(const QPoint& pos);
	void processRemove(const QPoint& pos);
	void beginProcessView(QMouseEvent *event);
	void beginProcessItem(QMouseEvent *event);
	void processItem(QMouseEvent *event);
	void panView(QMouseEvent *event);
	void doMoveItem(const QPoint& mousePos);
	void doMoveConnection(const QPoint& mousePos);
	void doConnectItem(QGraphicsItem* item);
	void doRemoveConnection(QGraphicsItem* item);
	
	enum Mode {
		mNone = 0,
		mPanView = 1,
		mMoveItem = 2,
		mConnectItems = 3,
		mRemoveConnection = 4,
	}; 
	
	Mode m_mode;
	QGraphicsItem * m_selectedItem;
	QPoint m_lastMosePos;
/// changed after pan
	QPoint m_sceneOrigin;
	
};

}

#endif