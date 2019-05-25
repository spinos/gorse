/*
 *  GlyphPort.h
 *  gorse
 *
 *  origin/destination of glyph connections
 *
 *  2019/5/25
 */

#ifndef ALO_GLYPH_PORT_H
#define ALO_GLYPH_PORT_H

#include <QGraphicsEllipseItem>
#include <vector>
#include <string>

namespace alo {

class GlyphConnection;

class GlyphPort : public QGraphicsEllipseItem
{

public:
	enum { Type = UserType + 2 };
	
	GlyphPort(QGraphicsItem * parent = 0 );
	virtual ~GlyphPort();
	
	void addConnection(GlyphConnection * conn);
	void removeConnection(GlyphConnection * conn2rm);
	void updateConnectionsPath();
	
	void setPortName(const QString & name);
	void setIsOutgoing(bool x);
	
	const QString & portName() const;
	std::string portNameStr() const;
	const bool & isOutgoing() const;
	
	int numConnections() const;
	const GlyphConnection * connection(const int & i) const;
    void getConnections(std::vector<GlyphConnection *> &conns) const;
    int getLastConnectionId() const;
	
	int type() const { return Type; }

	void genToolTip();
	
	static bool IsItemPort(const QGraphicsItem *item);
	static bool IsItemOutgoingPort(const QGraphicsItem *item);
	static bool IsItemIncomingPort(const QGraphicsItem *item);
	
protected:
	
private:
	QVector<GlyphConnection*> m_connections;
	QString m_portName;
	bool m_isOutgoing;
	
};

}
#endif