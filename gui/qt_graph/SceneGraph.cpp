/*
 *  SceneGraph.cpp
 *  gorse
 *
 *  2019/5/24
 */

#include "SceneGraph.h"
#include <QtWidgets>
#include "GlyphScene.h"
#include "GlyphConnection.h"
#include "GlyphHalo.h"
#include "GlyphPort.h"
#include "GlyphItem.h"
#include <qt_base/StateControlItem.h>

namespace alo {

SceneGraph::SceneGraph(GlyphScene *scene, QWidget * parent) : QGraphicsView(scene, parent)
{
	setAcceptDrops(true);
	setMinimumSize(400, 300);
	setRenderHint(QPainter::Antialiasing);
	setSceneRect(	frameRect() );
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
		case mEditState :
			endProcessItem(item);
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

		GlyphScene::CreateGlyphParameter prof;
		prof._type = pieceTyp;
		prof._id = -1;
		prof._pos = posmts;
		prof._isLoading = false;

		asGlyphScene()->createGlyph(prof);

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

/// different connections?
			m_selectedConnection = new GlyphConnection;

			GlyphPort *pt = static_cast<GlyphPort *>(item);
			m_selectedConnection->originFrom(pt);
			
			scene()->addItem(m_selectedConnection);

		} else {
		    QGraphicsItem *ti = item->topLevelItem();
			
			if(ti->type() == GlyphItem::Type ) {
				
				GlyphItem *selectedItem = static_cast<GlyphItem *>(ti);
				asGlyphScene()->selectGlyph(selectedItem);	

				if (item->type() == StateControlItem::Type) {
					selectedItem->beginEditState(item);
					m_mode = mEditState;
				} else
					m_mode = mMoveItem;
						
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
	if(g) {
        modifyDv(dv, g);
        g->moveBlockBy(dv);
    }
}

void SceneGraph::doMoveConnection(const QPoint& mousePos)
{
	if(!m_selectedConnection) return;
		
	QPointF pf(mousePos.x(), mousePos.y() );
	pf.rx() -= m_sceneOrigin.x();
	pf.ry() -= m_sceneOrigin.y();
	m_selectedConnection->updatePathTo(pf);
}

void SceneGraph::doConnectItem(QGraphicsItem* item)
{
	if(!m_selectedConnection) return;
		
	if(GlyphPort::IsItemIncomingPort(item)) {
		GlyphPort * p1 = static_cast<GlyphPort *>(item);
		bool connected = asGlyphScene()->makeConnection(m_selectedConnection, p1);
		if(connected) {
			QGraphicsItem *ti = p1->topLevelItem();
    		updateItemToolTip(ti);
		}
	}
	
	if(!m_selectedConnection->isComplete() ) {
		scene()->removeItem( m_selectedConnection );
/// graphic scene stops responding if destroy the connection
/// send to garbage?
		//delete m_selectedConnection;
	}
	
	m_selectedConnection = nullptr;
}

void SceneGraph::doRemoveConnection(QGraphicsItem* item)
{
	GlyphConnection *conn = static_cast<GlyphConnection *>(item);
	GlyphItem * dsi = conn->node1();
	asGlyphScene()->removeConnection(conn);
	scene()->removeItem( item );
	updateItemToolTip(dsi);
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

void SceneGraph::endProcessItem(QGraphicsItem *item)
{
	if (!item) return;
	QGraphicsItem *ti = item->topLevelItem();
			
	if(ti->type() != GlyphItem::Type ) return;
		
	GlyphItem *selectedItem = static_cast<GlyphItem *>(ti);
		
	if (item->type() == StateControlItem::Type) {
		asGlyphScene()->onItemStateChanged(selectedItem, item);
	}

}

void SceneGraph::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Delete) 
    	asGlyphScene()->removeActiveGlyph();
}

bool SceneGraph::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip) {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
        
        QGraphicsItem *item = itemAt(helpEvent->pos());
        if (item) {
            QToolTip::showText(helpEvent->globalPos(), item->toolTip() );
        } else {
            QToolTip::hideText();
            event->ignore();
        }

        return true;
    }
    return QGraphicsView::event(event);
}

void SceneGraph::modifyDv(QPointF &dv, const GlyphItem *focused) const
{
    const QPolygonF poly = focused->getCollisionPolygon(dv);
    const QPointF center = focused->getCollisionCenter();
        
    QList<QGraphicsItem *> dangers = scene()->items(poly);
    foreach (QGraphicsItem *item, dangers) {
        if (item == focused)
            continue;
        
        if(item->type() == GlyphItem::Type) {
            GlyphItem *dangerItem = static_cast<GlyphItem *>(item);
            
            QPointF toItem = dangerItem->getCollisionCenter() - center;
            float dl = sqrt(toItem.x() * toItem.x() + toItem.y() * toItem.y());
            if(dl > 0.f) toItem *= 1.f / dl;
            const float dt = toItem.x() * dv.x() + toItem.y() * dv.y();
            
            if(dt > 0.f) {
/// deflect
                dv -= toItem * dt;
            }
        }  
    }
    
}

void SceneGraph::updateItemToolTip(QGraphicsItem *item)
{
	if(item->type() == GlyphItem::Type ) {  
        GlyphItem *g = static_cast<GlyphItem *>(item);
        g->genToolTip();      
    }
}

} /// end of namespace alo
