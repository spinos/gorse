/*
 *  GlyphConnection.cpp
 *  gorse
 *
 *  2019/5/25
 */

#include "GlyphConnection.h"
#include "GlyphPort.h"
#include "GlyphItem.h"
#include <QPen>
#include <QBrush>
#include <iostream>

namespace alo {

GlyphConnection::GlyphConnection(QGraphicsItem * parent) : QGraphicsPathItem(parent),
m_cid(-1),
m_hintId(0),
m_port0(NULL),
m_port1(NULL)
{
	setPen(QPen(Qt::black, 2));
	setBrush(Qt::NoBrush);
	setZValue(0);
}

GlyphConnection::~GlyphConnection()
{
}

void GlyphConnection::setConnectionId(const int &x)
{ m_cid = x; }

void GlyphConnection::setPos0(const QPointF & p)
{ m_pos0 = p; }

void GlyphConnection::setPos1(const QPointF & p)
{ m_pos1 = p; }

void GlyphConnection::setPort0(GlyphPort * p)
{ 
	m_port0 = p;
	p->addConnection(this); 
}
	
void GlyphConnection::setPort1(GlyphPort * p)
{ 
	m_port1 = p; 
	p->addConnection(this);
}

void GlyphConnection::updatePathTo(const QPointF & pos)
{ 
    setPos1(pos);
    updatePath();
}

void GlyphConnection::updatePath()
{
	QPainterPath p;

	p.moveTo(m_pos0);

	qreal dx = m_pos1.x() - m_pos0.x();
	qreal dy = m_pos1.y() - m_pos0.y();

	QPointF ctr1(m_pos0.x() + dx * 0.125, m_pos0.y() + dy * 0.25);
	QPointF ctr2(m_pos0.x() + dx * 0.875, m_pos0.y() + dy * 0.75);

	p.cubicTo(ctr1, ctr2, m_pos1);

	setPath(p);
}

bool GlyphConnection::isComplete() const
{ 
	return (m_port0 && m_port1) && (m_port0 != m_port1); 
}

void GlyphConnection::updatePathByPort(GlyphPort * p)
{
	if(p==m_port0) {
		m_pos0 = p->scenePos();
	}
	if(p==m_port1) {
		m_pos1 = p->scenePos();
	}
	updatePath();
}

const int &GlyphConnection::connectionId() const
{ return m_cid; }

const int &GlyphConnection::hintId() const
{ return m_hintId; }

const GlyphPort * GlyphConnection::port0() const
{ return m_port0; }

const GlyphPort * GlyphConnection::port1() const
{ return m_port1; }

GlyphPort * GlyphConnection::port0()
{ return m_port0; }

GlyphPort * GlyphConnection::port1()
{ return m_port1; }

bool GlyphConnection::canConnectTo(GlyphPort* p1) const
{ 
	GlyphItem * srcNode = node0();
    GlyphItem * destNode = PortToNode(p1);
    return destNode->canConnectTo(srcNode, p1); 
}

bool GlyphConnection::IsItemConnection(const QGraphicsItem *item)
{
	if(!item) return false;
		
	return (item->type() == GlyphConnection::Type);
}

void GlyphConnection::originFrom(GlyphPort* p)
{
    setPort0(p);
    const QPointF pos = p->scenePos();
    setPos0(pos);
}

void GlyphConnection::destinationTo(GlyphPort* p1)
{
	m_hintId = p1->getLastConnectionId() + 1;

    GlyphItem * srcNode = node0();
    GlyphItem * destNode = PortToNode(p1);
    
    srcNode->preConnection(destNode, port0());
	destNode->preConnection(srcNode, p1);
    
    setPort1(p1);
    const QPointF pos = p1->scenePos();
    setPos1(pos);
	updatePath();
			
	destNode->postConnection(srcNode, p1, this);
    srcNode->postConnection(destNode, port0(), this);

}

void GlyphConnection::breakUp()
{
    GlyphItem * n1 = node1();
    GlyphItem * n0 = node0();
	if(m_port1) {
        
        n1->preDisconnection(n0, m_port1, this);
        m_port1->removeConnection(this);
        n1->postDisconnection(m_port1);
        m_port1 = nullptr;
	}
	
    if(m_port0) {
        
        n0->preDisconnection(n1, m_port0, this);
        m_port0->removeConnection(this);
        n0->postDisconnection(m_port0);
        m_port0 = nullptr;
	}

}

GlyphItem *GlyphConnection::node0() const
{ return PortToNode(port0()); }

GlyphItem *GlyphConnection::node1() const
{ return PortToNode(port1()); }

GlyphItem *GlyphConnection::PortToNode(const GlyphPort *pt)
{ return static_cast<GlyphItem *>(pt->parentItem() ); }

void GlyphConnection::sendImpulse()
{
	GlyphItem * n0 = node0();
	GlyphItem * n1 = node1();
	n1->onInputChange(n0, port1());
}

void GlyphConnection::genToolTip()
{
	GlyphItem * n0 = node0();
	QString name0 = QString::fromStdString(n0->glyphName());
	GlyphItem * n1 = node1();
	QString name1 = QString::fromStdString(n1->glyphName());
	QString stip = QString("%1.%2 -%3- %4.%5").arg(name0, m_port0->portName(), QString::number(m_cid), name1, m_port1->portName() );
	setToolTip(stip);
}

void GlyphConnection::getNodeIds(int *y) const
{
	const GlyphItem * n0 = node0();
	y[0] = n0->glyphId();
	const GlyphItem * n1 = node1();
	y[1] = n1->glyphId();
}

void GlyphConnection::getPortNames(std::string &y0, std::string &y1) const
{
	y0 = m_port0->portNameStr();
	y1 = m_port1->portNameStr();
}

}