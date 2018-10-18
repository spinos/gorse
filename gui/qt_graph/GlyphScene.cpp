/*
 *  GlyphScene.cpp
 *
 *
 *  Created by jian zhang on 3/30/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#include <QtGui>
#include "GlyphScene.h"
#include "GlyphPort.h"
#include "GlyphHalo.h"
#include "GlyphConnection.h"
#include "GlyphItem.h"
#include "GlyphOps.h"
#include <math/GroupCollection.h>

namespace alo {

GlyphScene::GlyphScene(GroupCollection<QJsonObject> *collector,
					QObject *parent) :
	QGraphicsScene(parent), 
	m_collector(collector),
	m_activeGlyph(0)
{}

GlyphScene::~GlyphScene()
{}

void GlyphScene::createGlyph(const QPixmap &pix, int typ, const QPointF & pos)
{
	GlyphItem * g = new GlyphItem(pix, typ);
	addItem(g);

	g->setPos(pos);

	GlyphHalo* hal = new GlyphHalo;
	QPointF posmts = pos + g->localCenter();
	hal->setPos(posmts.x() - 50, posmts.y() - 50 );
	addItem(hal);
	g->setHalo(hal);

	QJsonObject content = m_collector->element(typ);
	GlyphOps *ops = createOps(content);
	ops->addAttributes(content);
	g->setOps(ops);
}

GlyphOps *GlyphScene::createOps(const QJsonObject &content)
{ return new GlyphOps; }

void GlyphScene::selectGlyph(GlyphItem *item)
{
	if(!m_selectedGlyph.contains(item) )
		m_selectedGlyph<<item; 
	if(m_activeGlyph != item) {
		m_activeGlyph = item;
		item->postSelection();
		emit sendSelectGlyph(true);
	}
}

void GlyphScene::deselectGlyph()
{
	foreach(GlyphItem * gl, m_selectedGlyph) {
		gl->hideHalo();
	}
	m_activeGlyph = 0;
	m_selectedGlyph.clear();
	emit sendSelectGlyph(false);
}

GlyphOps *GlyphScene::getActiveOps() const
{
	if(!m_activeGlyph) return 0;
	return m_activeGlyph->getOps();
}

}
