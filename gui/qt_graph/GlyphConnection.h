/*
 *  GlyphConnection.h
 *
 *  from port0 to port1
 *
 *  Created by jian zhang on 4/1/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_GLYPH_CONNECTION_H
#define ALO_GLYPH_CONNECTION_H

#include <QGraphicsPathItem>

namespace alo {

class GlyphPort;
class GlyphItem;

class GlyphConnection : public QGraphicsPathItem
{
public:
	enum { Type = UserType + 3 };
	
	GlyphConnection(QGraphicsItem * parent = 0);
	virtual ~GlyphConnection();
	
	void updatePath();
    void updatePathTo(const QPointF & pos);
	void updatePathByPort(GlyphPort * p);
	bool isComplete() const;
	
	const GlyphPort * port0() const;
	const GlyphPort * port1() const;
	GlyphItem *node0() const;
	GlyphItem *node1() const;
	
    void originFrom(GlyphPort* p, const QPointF & pos);
	bool canConnectTo(GlyphPort* p1) const;
    void destinationTo(GlyphPort* p1);
	void breakUp();
	
	int type() const { return Type; }
	
	static bool IsItemConnection(const QGraphicsItem *item);
	
protected:
    void setPos0(const QPointF & p);
	void setPos1(const QPointF & p);
	void setPort0(GlyphPort * p);
	void setPort1(GlyphPort * p);
    
	GlyphPort *port0();
	GlyphPort *port1();
	static GlyphItem *PortToNode(const GlyphPort *pt);
	
private:
	QPointF m_pos0;
	QPointF m_pos1;
	GlyphPort * m_port0;
	GlyphPort * m_port1;
	
};

}

#endif