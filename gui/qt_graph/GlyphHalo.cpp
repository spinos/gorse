/*
 *  GlyphHalo.cpp
 *  aloe
 *
 */

#include <QtGui>
#include "GlyphHalo.h"

namespace alo {

GlyphHalo::GlyphHalo(QGraphicsItem * parent) : QGraphicsEllipseItem(parent)
{
	m_highlight = new QGraphicsEllipseItem(this);
	m_highlight->setRect(0, 0, 76, 76);
	m_highlight->setPos(12, 12);
	m_highlight->setPen(QPen(Qt::NoPen));
	m_highlight->setBrush(QColor(137, 88, 131) );

	setRect(0, 0, 100, 100);
	setPen(QPen(Qt::NoPen));
	setBrush(QColor(66, 101, 133) );
	setZValue(-1);
	hide();
}

GlyphHalo::~GlyphHalo()
{ delete m_highlight; }

void GlyphHalo::setHightVisible(bool x)
{
	if(x) m_highlight->show();
	else m_highlight->hide();
}

}