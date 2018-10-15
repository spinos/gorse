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

namespace alo {

GlyphItem::GlyphItem(const QPixmap & iconPix, int gtyp,
			QGraphicsItem * parent) : QGraphicsPathItem(parent)
{
	//setFlag(QGraphicsItem::ItemIsSelectable);
	resizeBlock(120, 36);
	setPen(QPen(Qt::darkGray));
	setBrush(Qt::lightGray);
	setZValue(1);
	//m_halo->setPos(60-50, 18-50);
	m_icon = new QGraphicsPixmapItem(iconPix.scaled(32,32), this);
	m_icon->setPos(60-16, 18-16);
	m_glyphType = gtyp;
	m_ops = 0;
}

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

void GlyphItem::movePorts(int n, bool downSide)
{
	if(n < 1) {
		return;
	}
	
	int py = -7;
	if(downSide) {
		py = m_blockHeight + 7;
	}
	
	int px = m_blockWidth / 2 - 30 * (n / 2);
	if((n & 1) == 0) {
		px += 15;
	}
	
	foreach(QGraphicsItem *port_, childItems()) {
		if (port_->type() != GlyphPort::Type) {
			continue;
		}

		GlyphPort *port = (GlyphPort*) port_;
		
		if(port->isOutgoing() == downSide ) {
			
			port->setPos(px, py);
			px += 30;
		}
	}
}	

GlyphPort * GlyphItem::addPort(const QString & name, 
							bool isOutgoing)
{
	GlyphPort * pt = new GlyphPort(this);
	pt->setPortName(name);
	pt->setIsOutgoing(isOutgoing);
	return pt;
}

void GlyphItem::finalizeShape()
{
	int nincoming = 0;
	int noutgoing = 0;
	foreach(QGraphicsItem *port_, childItems()) {
		if (port_->type() != GlyphPort::Type) {
			continue;
		}

		GlyphPort *port = (GlyphPort*) port_;
		
		if(port->isOutgoing() ) {
			noutgoing++;
		} else {
			nincoming++;
		}
	}

	int wup = nincoming * 30;
	int wdown = noutgoing * 30;
	if(wup > wdown) {
		wup = wdown;
	}
	if(wup > 120) {
		resizeBlock(wup, 36);
		centerIcon();
	}
	
	movePorts(nincoming, false);
	movePorts(noutgoing, true);
	
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

void GlyphItem::setGlyphType(int x)
{
	m_glyphType = x;
}

const int & GlyphItem::glyphType() const
{
	return m_glyphType;
}

void GlyphItem::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{ event->ignore(); }

void GlyphItem::mouseDoubleClickEvent( QGraphicsSceneMouseEvent * event )
{ event->ignore(); }

void GlyphItem::setHalo(GlyphHalo* hal)
{ m_halo = hal; }

void GlyphItem::showHalo()
{ m_halo->show(); }

void GlyphItem::hideHalo()
{ m_halo->hide(); }

GlyphHalo* GlyphItem::halo()
{ return m_halo; }

QPointF GlyphItem::localCenter() const
{ return QPointF(m_blockWidth / 2, m_blockHeight / 2); }

void GlyphItem::setOps(GlyphOps *ops)
{ m_ops = ops; }

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
    m_ops->update(); 
    showHalo();
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
