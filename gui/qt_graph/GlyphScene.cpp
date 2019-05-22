/*
 *  GlyphScene.cpp
 *  gorse
 *
 *  2019/5/20
 */

#include "GlyphScene.h"
#include "GlyphPort.h"
#include "GlyphHalo.h"
#include "GlyphConnection.h"
#include "GlyphItem.h"
#include "GlyphOps.h"
#include "AttribCreator.h"
#include <qt_base/ActivationControlItem.h>
#include <qt_base/VisibilityControlItem.h>
#include <qt_base/ImageCollector.h>
#include <math/GroupCollection.h>
#include <QDebug>

namespace alo {

GlyphScene::GlyphScene(QObject *parent) :
	QGraphicsScene(parent), 
	m_collector(nullptr),
	m_activeGlyph(nullptr),
	m_isLoading(false)
{
}

GlyphScene::~GlyphScene()
{
}

void GlyphScene::setAssetCollection(GroupCollection<QJsonObject> *x)
{ m_collector = x; }

void GlyphScene::initializeGraphics()
{
	VisibilityControlItem::InitializeStates();
    ActivationControlItem::InitializeStates();
}

void GlyphScene::createGlyph(const GlyphProfile &param)
{
	QJsonObject content = m_collector->element(param._type);
	QString iconName;
    if(content.find("icon") == content.end()) 
        iconName = ":/images/unknown.png";
    else 
        iconName = content["icon"].toString();
    QPixmap pix = ImageCollector::CollectPixmap(iconName);

	GlyphItem *g = new GlyphItem(pix, param._type);
	addItem(g);

	const int gid = param._id < 1 ? getUid(param._type) : param._id;
	g->setGlyphId(gid);
	m_glyphMap.insert(gid, g);

	g->setPos(param._pos);

	GlyphHalo* hal = new GlyphHalo;
	QPointF posmts = param._pos + g->localCenter();
	hal->setPos(posmts.x() - 50, posmts.y() - 50 );
	addItem(hal);
	g->setHalo(hal);

	GlyphOps *ops = createOps(content);
	ops->setGlyphScene(this);

	AttribCreator acr;
	acr.addAttributes(ops, content);

	g->setOps(ops);
	if(ops->hasDrawable() || ops->hasRenderable()) 
        g->addVisibilityControl();
    if(ops->hasEnable()) 
        g->addEnableControl();

    if(param._isLoading)
    	postCreationBlocked(g);
	else 
		postCreation(g);
}

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
		emit sendGlyphIcon(item->iconPix());
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

void GlyphScene::removeActiveGlyph()
{
	if(!m_activeGlyph) return;

	const int &k = m_activeGlyph->glyphId();
	deselectGlyph(m_activeGlyph);

	GlyphHalo* h = m_activeGlyph->halo();
	QGraphicsScene::removeItem(h);
	delete h;

	std::vector<GlyphConnection *> conns;
	m_activeGlyph->getConnections(conns);

	preDestruction(m_activeGlyph, conns);

	foreach(GlyphConnection *conn, conns) {
		QGraphicsScene::removeItem(conn);
		//delete conn;
	}

	QGraphicsScene::removeItem(m_activeGlyph);

	m_glyphMap.remove(k);

	delete m_activeGlyph;
	m_activeGlyph = 0;
	emit sendSelectGlyph(false);
}

bool GlyphScene::glyphExists(const int i)
{ return m_glyphMap.find(i) != nullptr; }

GlyphScene::GlyphDataType *GlyphScene::firstGlyph() const
{ return m_glyphMap.begin(); }

GlyphScene::GlyphDataType *GlyphScene::nextGlyph(const GlyphDataType *x) const
{ return m_glyphMap.next(x); }

void GlyphScene::onFocusIn3D(const Float4 &centerRadius)
{ emit sendFocusCameraOn(centerRadius); }

int GlyphScene::getUid(const int typeId)
{
	int *c = m_typeCounter.find(typeId);
	if(!c) c = m_typeCounter.insert(typeId, 0);

	*c += 1;
	return ((typeId<<10) | *c);
}

void GlyphScene::resetGlyphScene()
{
	QGraphicsScene::clear();
	m_activeGlyph = nullptr;
	m_selectedGlyph.clear();
	m_typeCounter.clear();
	m_glyphMap.clear();
}

QJsonObject GlyphScene::getGlyphProfile(int typeId)
{ return m_collector->element(typeId); }

void GlyphScene::preLoad()
{
	resetGlyphScene();
}

void GlyphScene::postLoad()
{}

} /// end of alo
