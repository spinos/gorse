/*
 *  GlyphItem.cpp
 *  
 *
 *  Created by jian zhang on 3/31/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#include <QtWidgets>
#include <QGraphicsSceneMouseEvent>
#include "GlyphItem.h"
#include "GlyphPort.h"
#include "GlyphHalo.h"
#include "GlyphOps.h"
#include "Attrib.h"
#include <qt_base/AFileDlg.h>

namespace alo {

GlyphItem::GlyphItem(const QPixmap & iconPix, int gtyp,
			QGraphicsItem * parent) : QGraphicsPathItem(parent)
{
	//setFlag(QGraphicsItem::ItemIsSelectable);
	resizeBlock(120, 40);
	setPen(QPen(Qt::darkGray));
	setBrush(Qt::lightGray);
	setZValue(1);
	//m_halo->setPos(60-50, 18-50);
	m_icon = new QGraphicsPixmapItem(iconPix.scaled(32,32), this);
	m_icon->setPos(4, 4);
	m_glyphType = gtyp;
	m_ops = 0;
}

void GlyphItem::setGlyphId(int x)
{ m_glyphId = x; }

void GlyphItem::resizeBlock(int bx, int by)
{
	QPainterPath p;
	p.addRoundedRect(0, 0, bx, by, 4, 4);
	setPath(p);
	m_blockWidth = bx;
	m_blockHeight = by;
}

void GlyphItem::centerIcon()
{
	m_icon->setPos(m_blockWidth/2 - 16, m_blockHeight/2 - 16);
}	

GlyphPort * GlyphItem::addPort(const QString & name, 
							bool isOutgoing)
{
	GlyphPort * pt = new GlyphPort(this);
	pt->setPortName(name);
	pt->setIsOutgoing(isOutgoing);
	return pt;
}

void GlyphItem::moveBlockBy(const QPointF & dp)
{
	foreach(QGraphicsItem *port_, childItems()) {
		if (port_->type() != GlyphPort::Type) {
			continue;
		}

		GlyphPort *port = (GlyphPort*) port_;
		port->updateConnectionsPath();
		
	}
	moveBy(dp.x(), dp.y() );
	m_halo->moveBy(dp.x(), dp.y() );
}

const int & GlyphItem::glyphType() const
{ return m_glyphType; }

const int &GlyphItem::glyphId() const
{ return m_glyphId; }

void GlyphItem::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{ event->ignore(); }

void GlyphItem::mouseDoubleClickEvent( QGraphicsSceneMouseEvent * event )
{ event->ignore(); }

void GlyphItem::contextMenuEvent(QGraphicsSceneContextMenuEvent * event)
{
	if(!m_ops->hasMenu()) {
		event->ignore();
		return;
    }

    std::vector<std::pair<std::string, int > > ks;
    m_ops->getMenuItems(ks);
    
	QMenu menu;
	std::vector<std::pair<std::string, int > >::const_iterator it = ks.begin();
	for(;it!=ks.end();++it) {
		QAction *a = menu.addAction(QString(it->first.c_str()));
		a->setData(QVariant(it->second));
	}
	ks.clear();
    
    QAction *selectedAction = menu.exec(event->screenPos());
    if(selectedAction)
    	processContextMenu(selectedAction->data().toInt() );

}

void GlyphItem::setHalo(GlyphHalo* hal)
{ m_halo = hal; }

void GlyphItem::showHalo()
{ m_halo->show(); }

void GlyphItem::hideHalo()
{ m_halo->hide(); }

GlyphHalo* GlyphItem::halo()
{ return m_halo; }

GlyphOps *GlyphItem::getOps() const
{ return m_ops; }

GlyphOps *GlyphItem::ops()
{ return m_ops; }

QPointF GlyphItem::localCenter() const
{ return QPointF(m_blockWidth / 2, m_blockHeight / 2); }

void GlyphItem::setOps(GlyphOps *ops)
{ 
	m_ops = ops;
	std::map<std::string, QAttrib * >::iterator it = ops->attribBegin();
	for(;it != ops->attribEnd();++it) {
		const Connectable *c = it->second->connection();
		if(c) {
			GlyphPort *pt = addPort(QString(it->first.c_str()), c->_isOutgoing );
			movePort(pt, c);
		}
	}
}

void GlyphItem::movePort(GlyphPort *pt, const Connectable *c)
{
	int px, py;
	switch(c->_side) {
		case ConnectLeft :
			px = c->_px - 8;
			py = c->_py;
			break;
		case ConnectRight :
			px = m_blockWidth + c->_px + 8;
			py = c->_py;
			break;
		case ConnectTop :
			px = c->_px;
			py = c->_py - 8;
			break;
		case ConnectBottom :
			px = c->_px;
			py = m_blockHeight + 8;
			break;
		default :
			break;
	}

	pt->setPos(px, py);
}

void GlyphItem::processContextMenu(int k)
{
	AFileDlgProfile *prof = 0;
	if(k==AFileDlgProfile::FWrite)
		prof = m_ops->writeFileProfileR();
	else if (k==AFileDlgProfile::FWrite)
		prof = m_ops->readFileProfileR();

	if(!prof) return;

	AFileDlg d(*prof, QApplication::activeWindow() );
    int res = d.exec();
    if(res == QDialog::Rejected) {
        qDebug()<<" abort ";
        return;
    }

    m_ops->recvAction(k);
    	
}

const std::string& GlyphItem::glyphName() const
{ return "";//m_ops->glyphName(); 
}

void GlyphItem::postConnection(GlyphItem* another, GlyphPort* viaPort)
{ 
	//m_ops->connectTo(another->attrib(), 
	//			viaPort->portName().toStdString() ); 
}

void GlyphItem::postSelection()
{ 
    showHalo();
    m_ops->onSelection();
}

void GlyphItem::preDisconnection(GlyphItem* another, GlyphPort* viaPort)
{
	//m_ops->disconnectFrom(another->attrib(),
	//				viaPort->portName().toStdString() );
}

void GlyphItem::postDisconnection(GlyphPort* viaPort)
{
	//m_ops->postPortChange(viaPort->portName().toStdString() );
}

}
