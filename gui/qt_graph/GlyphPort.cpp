/*
 *  GlyphPort.cpp
 *  gorse
 *
 *  2019/5/25
 */

#include "GlyphPort.h"
#include "GlyphConnection.h"
#include <QPen>
#include <QBrush>

namespace alo {

GlyphPort::GlyphPort(QGraphicsItem * parent) : QGraphicsEllipseItem(parent)
{
	setRect(-7, -7, 14, 14);
	setPen(QPen(Qt::darkGray));
	setBrush(Qt::lightGray);
	setZValue(2);
}

GlyphPort::~GlyphPort()
{
}

void GlyphPort::setPortName(const QString & name)
{ m_portName = name; }

void GlyphPort::setIsOutgoing(bool x)
{ m_isOutgoing = x; }

const QString & GlyphPort::portName() const
{ return m_portName; }

std::string GlyphPort::portNameStr() const
{ return m_portName.toStdString(); }

const bool & GlyphPort::isOutgoing() const
{ return m_isOutgoing; }

void GlyphPort::addConnection(GlyphConnection * conn)
{   
    m_connections.append(conn);
}

void GlyphPort::removeConnection(GlyphConnection * conn2rm)
{
	int found = -1;
	int i=0;
	foreach(GlyphConnection *conn, m_connections) {
		if(conn == conn2rm) {
			found = i;
		}
		i++;
	}
	if(found > -1) {
		m_connections.remove(found);
	}
}

void GlyphPort::updateConnectionsPath()
{
	foreach(GlyphConnection *conn, m_connections) {
		conn->updatePathByPort(this);
	}
}

int GlyphPort::numConnections() const
{
	return m_connections.size();
}

const GlyphConnection * GlyphPort::connection(const int & i) const
{ return m_connections[i]; }

void GlyphPort::getConnections(std::vector<GlyphConnection *> &conns) const
{
    foreach(GlyphConnection *conn, m_connections) {
		conns.push_back(conn);
	}
}

int GlyphPort::getLastConnectionId() const
{
	if(numConnections() < 1) return -1;
	int y = m_connections.last()->connectionId();
	return y;
}

bool GlyphPort::IsItemPort(const QGraphicsItem *item)
{
	if(!item) return false;
	
	return (item->type() == GlyphPort::Type);
}

bool GlyphPort::IsItemOutgoingPort(const QGraphicsItem *item)
{
	if(!IsItemPort(item) ) return false;

	const GlyphPort * pt = (const GlyphPort *)item;
	return pt->isOutgoing();
}

bool GlyphPort::IsItemIncomingPort(const QGraphicsItem *item)
{
	if(!IsItemPort(item) ) return false;
	
	const GlyphPort * pt = (const GlyphPort *)item;
	return !pt->isOutgoing();
}

void GlyphPort::genToolTip()
{
	QString sdir = m_isOutgoing ? "outgoing" : "incoming";
	QString stip = QString("%1 port: %2").arg(sdir, m_portName);
	setToolTip(stip);
}

}