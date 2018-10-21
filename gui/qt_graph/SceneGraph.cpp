/*
 *  SceneGraph.cpp
 *  
 *
 *  Created by jian zhang on 3/30/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#include <QtGui>
#include "SceneGraph.h"
#include "GlyphScene.h"
#include "GlyphConnection.h"
#include "GlyphHalo.h"
#include "GlyphPort.h"
#include "GlyphItem.h"

namespace alo {

SceneGraph::SceneGraph(GlyphScene *scene, QWidget * parent) : QGraphicsView(scene, parent)
{
	setAcceptDrops(true);
	setMinimumSize(400, 300);
	setRenderHint(QPainter::Antialiasing);
	setSceneRect(	frameRect() );
	//m_selectedItem = 0;
	m_selectedConnection = 0;
}

GlyphScene *SceneGraph::asGlyphScene()
{ return static_cast<GlyphScene *>(scene()); }

void SceneGraph::mousePressEvent(QMouseEvent *event)
{
	if(event->modifiers() == Qt::AltModifier) {
		beginProcessView(event);		
	} else {
		beginProcessItem(event);
	}
}

void SceneGraph::mouseMoveEvent(QMouseEvent *event)
{
	if(m_mode == mPanView) {
		panView(event);
	} else {
		processItem(event);
	}
}

void SceneGraph::panView(QMouseEvent *event)
{
	QPoint mousePos = event->pos();
	QPointF dv(mousePos.x() - m_lastMosePos.x(),
					mousePos.y() - m_lastMosePos.y() );
					
	QRectF frm = sceneRect ();
	frm.adjust(-dv.x(), -dv.y(), -dv.x(), -dv.y() );
	m_sceneOrigin.rx() += dv.x();
	m_sceneOrigin.ry() += dv.y();
	setSceneRect(frm); 
	
	m_lastMosePos = mousePos;
}

void SceneGraph::processItem(QMouseEvent *event)
{
	QPoint mousePos = event->pos();
	QGraphicsItem *item = itemAt(mousePos);
	
	switch (m_mode) {
		case mMoveItem :
			doMoveItem(mousePos);
			break;
		case mConnectItems :
			doMoveConnection(mousePos);
			break;
		default :
			;
	}
	
	m_lastMosePos = mousePos;
}
	
void SceneGraph::mouseReleaseEvent(QMouseEvent *event)
{
	QPoint mousePos = event->pos();
	QGraphicsItem *item = itemAt(mousePos);
	
	switch (m_mode) {
		case mMoveItem :
			doMoveItem(mousePos);
			break;
		case mConnectItems :
			doConnectItem(item);
			break;
		default :
			;
	}
	m_mode = mNone;
}

void SceneGraph::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasFormat("application/x-dndpiecedata"))
        event->accept();
    else
        event->ignore();
}

void SceneGraph::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dndpiecedata")
        //&& findPiece(targetSquare(event->pos())) == -1
		) {

        //highlightedRect = targetSquare(event->pos());
        event->setDropAction(Qt::MoveAction);
        event->accept();
    } else {
        //highlightedRect = QRect();
        event->ignore();
    }

    /*update(updateRect);*/
}

void SceneGraph::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dndpiecedata") ) {

		QByteArray pieceData = event->mimeData()->data("application/x-dndpiecedata");
		QDataStream dataStream(&pieceData, QIODevice::ReadOnly);
		QPixmap pixmap;
		int pieceTyp;
		dataStream >> pixmap >> pieceTyp;
		
		QPointF posmts = mapToScene(event->pos() );
		asGlyphScene()->createGlyph(pixmap, pieceTyp, posmts );

		event->setDropAction(Qt::MoveAction);
		event->accept();
    } else {
		event->ignore();
	}
	
}

void SceneGraph::resizeEvent ( QResizeEvent * event )
{
	setSceneRect(QRectF(-m_sceneOrigin.x(), -m_sceneOrigin.y(), 
		event->size().width(), event->size().height() ));
	QGraphicsView::resizeEvent(event);
}

void SceneGraph::processSelect(const QPoint & pos)
{
	QGraphicsItem *item = itemAt(pos);
	if (item) {
         if(GlyphPort::IsItemOutgoingPort(item) ) {
			m_mode = mConnectItems;
/// todo different connections
			m_selectedConnection = new GlyphConnection;
			m_selectedConnection->setPos0(item->scenePos() );
			
			GlyphPort * pt = static_cast<GlyphPort *>(item);
			m_selectedConnection->setPort0(pt);
			
			scene()->addItem(m_selectedConnection);
			
		 } else {
		     QGraphicsItem *ti = item->topLevelItem();
			
			if(ti->type() == GlyphItem::Type ) {
				m_mode = mMoveItem;
				GlyphItem *selectedItem = static_cast<GlyphItem *>(ti);
				asGlyphScene()->selectGlyph(selectedItem);				
			}
		 }
     } else {
         asGlyphScene()->deselectAllGlyph();
	 }
	 m_lastMosePos = pos;
}

void SceneGraph::beginProcessView(QMouseEvent *event) 
{
	QPoint mousePos = event->pos();
	m_mode = mPanView;
	m_lastMosePos = mousePos;
}

void SceneGraph::processRemove(const QPoint& pos)
{
	QGraphicsItem *item = itemAt(pos);
	if (item) {
         if(GlyphConnection::IsItemConnection(item) ) {
			m_mode = mRemoveConnection;
			doRemoveConnection(item);
		 }
     }
	 m_lastMosePos = pos;
}

void SceneGraph::doMoveItem(const QPoint& mousePos)
{
	QPointF dv(mousePos.x() - m_lastMosePos.x(),
					mousePos.y() - m_lastMosePos.y() );
	GlyphItem *g = asGlyphScene()->getActiveGlyph();
	if(g) g->moveBlockBy(dv);
}

void SceneGraph::doMoveConnection(const QPoint& mousePos)
{
	if(!m_selectedConnection)
		return;
		
	QPointF pf(mousePos.x(), mousePos.y() );
	pf.rx() -= m_sceneOrigin.x();
	pf.ry() -= m_sceneOrigin.y();
	m_selectedConnection->setPos1(pf );
	m_selectedConnection->updatePath();
}

void SceneGraph::doConnectItem(QGraphicsItem* item)
{
	if(!m_selectedConnection)
		return;
		
	if(GlyphPort::IsItemIncomingPort(item)) {
		QPointF pf = item->scenePos();
		m_selectedConnection->setPos1(pf );
		GlyphPort * p1 = (GlyphPort *)item;
		if(m_selectedConnection->canConnectTo(p1) ) {
			m_selectedConnection->setPort1(p1);
			m_selectedConnection->updatePath();
			
			GlyphItem * srcNode = m_selectedConnection->node0();
			GlyphItem * destNode = m_selectedConnection->node1();
			destNode->postConnection(srcNode, p1);
			//qDebug()<<" made connection "<<m_selectedConnection->port0()->portName()
			//    <<" -> "<<pt->portName();				
		} else {		
			//qDebug()<<" rejects connection "<<m_selectedConnection->port0()->parentItem()
			//<<" -> "<<pt->parentItem();
		}
	}
	
	if(!m_selectedConnection->isComplete() ) {
		scene()->removeItem( m_selectedConnection );
		delete m_selectedConnection;
	}
	m_selectedConnection = NULL;
}

void SceneGraph::doRemoveConnection(QGraphicsItem* item)
{
	GlyphConnection *conn = static_cast<GlyphConnection *>(item);
	conn->breakUp();
	scene()->removeItem( item );
	delete item;
}

void SceneGraph::beginProcessItem(QMouseEvent *event) 
{
	switch (event->button())
	{
		case Qt::LeftButton:
			processSelect(event->pos() );
		break;
		case Qt::RightButton:
			processRemove(event->pos() );
		break;
		default:
		;
	}
}

void SceneGraph::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Delete) 
    	asGlyphScene()->removeActiveItem();
}

void SceneGraph::removeActiveItem()
{
	GlyphItem *g = asGlyphScene()->getActiveGlyph();
	if(!g) return;
	scene()->removeItem(g);
	delete g;
}

}
