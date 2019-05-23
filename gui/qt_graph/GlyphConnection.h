/*
 *  GlyphConnection.h
 *  gorse
 *
 *  from node0.port0 to nod1.port1
 *
 *  2019/5/25
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
    
    void setConnectionId(const int &x);
	
	void updatePath();
    void updatePathTo(const QPointF & pos);
	void updatePathByPort(GlyphPort * p);
	bool isComplete() const;
	
    const int &connectionId() const;
/// suggestion of the minimal
    const int &hintId() const;
	const GlyphPort * port0() const;
	const GlyphPort * port1() const;
	GlyphItem *node0() const;
	GlyphItem *node1() const;
	void getNodeIds(int *y) const;
	void getPortNames(std::string &y0, std::string &y1) const;
	
    void originFrom(GlyphPort* p);
	bool canConnectTo(GlyphPort* p1) const;
    void destinationTo(GlyphPort* p1);
	void breakUp();
	void sendImpulse();
	
	int type() const { return Type; }

	void genToolTip();
	
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

    int m_cid;
    int m_hintId;
	QPointF m_pos0;
	QPointF m_pos1;
	GlyphPort * m_port0;
	GlyphPort * m_port1;
	
};

}

#endif