/*
 *  GlyphScene.cpp
 *
 *
 *  Created by jian zhang on 3/30/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#include "GlyphScene.h"
#include "GlyphPort.h"
#include "GlyphHalo.h"
#include "GlyphConnection.h"
#include "GlyphItem.h"
#include "GlyphOps.h"
#include "VisibilityControlItem.h"
#include <math/GroupCollection.h>
#include <ctime>

namespace alo {

GlyphScene::GlyphScene(QObject *parent) :
	QGraphicsScene(parent), 
	m_collector(nullptr),
	m_activeGlyph(nullptr)
{
	time_t now;
	time(&now);
	m_rng = new Uniform<Lehmer>((unsigned)now);
}

GlyphScene::~GlyphScene()
{
	delete m_rng;
}

void GlyphScene::setAssetCollection(GroupCollection<QJsonObject> *x)
{ m_collector = x; }

void GlyphScene::initializeGraphics()
{
	VisibilityControlItem::InitializeStates();
}

void GlyphScene::createGlyph(const QPixmap &pix, int typ, const QPointF & pos)
{
	GlyphItem *g = new GlyphItem(pix, typ);
	addItem(g);

	const int gid = getUid();
	g->setGlyphId(gid);
	m_glyphMap.insert(gid, g);

	g->setPos(pos);

	GlyphHalo* hal = new GlyphHalo;
	QPointF posmts = pos + g->localCenter();
	hal->setPos(posmts.x() - 50, posmts.y() - 50 );
	addItem(hal);
	g->setHalo(hal);

	QJsonObject content = m_collector->element(typ);
	GlyphOps *ops = createOps(content);
	ops->setGlyphScene(this);
	ops->addAttributes(content);
	g->setOps(ops);
	if(ops->hasDrawable() || ops->hasRenderable()) g->addVisibilityControl();
	postCreation(g);
}

GlyphOps *GlyphScene::createOps(const QJsonObject &content)
{ return new GlyphOps; }

void GlyphScene::selectGlyph(GlyphItem *item)
{
    foreach(GlyphItem * glh, m_selectedGlyph)
		glh->deactivateHalo();

	if(!m_selectedGlyph.contains(item) )
		m_selectedGlyph<<item; 
    
    item->activateHalo();
    
	if(m_activeGlyph != item) {
		m_activeGlyph = item;
		item->postSelection();
		emit sendSelectGlyph(true);
	}
}

void GlyphScene::deselectAllGlyph()
{
	foreach(GlyphItem * glh, m_selectedGlyph) {
		glh->hideHalo();
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

GlyphItem *GlyphScene::getActiveGlyph() const
{ return m_activeGlyph; }

void GlyphScene::deselectGlyph(GlyphItem *item)
{
	QList<GlyphItem *>::iterator it = m_selectedGlyph.begin();
	for(;it!=m_selectedGlyph.end();++it) {
		if(*it == item) break;
	}

	if(it != m_selectedGlyph.end()) {
		(*it)->hideHalo();
		m_selectedGlyph.erase(it);
	}
}

void GlyphScene::removeActiveItem()
{
	if(!m_activeGlyph) return;

	const int &k = m_activeGlyph->glyphId();
	deselectGlyph(m_activeGlyph);

	GlyphHalo* h = m_activeGlyph->halo();
	QGraphicsScene::removeItem(h);
	delete h;

	preDestruction(m_activeGlyph);

	QGraphicsScene::removeItem(m_activeGlyph);

	m_glyphMap.remove(k);

	delete m_activeGlyph;
	m_activeGlyph = 0;
	emit sendSelectGlyph(false);
}

int GlyphScene::getUid()
{ 
	int r = m_rng->rand(1<<26); 
	while(m_glyphMap.find(r))
		r = m_rng->rand(1<<26); 
	
	return r;
}

GlyphScene::GlyphDataType *GlyphScene::firstGlyph()
{ return m_glyphMap.begin(); }

GlyphScene::GlyphDataType *GlyphScene::nextGlyph(const GlyphDataType *x)
{ return m_glyphMap.next(x); }

void GlyphScene::onFocusIn3D(const Float4 &centerRadius)
{ emit sendFocusCameraOn(centerRadius); }

}
