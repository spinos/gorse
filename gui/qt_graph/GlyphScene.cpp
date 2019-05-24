/*
 *  GlyphScene.cpp
 *  gorse
 *
 *  2019/5/25
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
	m_activeGlyph(nullptr)
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

GlyphItem *GlyphScene::createGlyph(CreateGlyphParameter &param)
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
    param._ops = ops;

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
	return g;
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
	m_activeGlyph->getConnections(conns, false);

	preDestruction(m_activeGlyph, conns);

	foreach(GlyphConnection *conn, conns) {
		QGraphicsScene::removeItem(conn);
		unmapConnection(conn);
	}

	QGraphicsScene::removeItem(m_activeGlyph);

	m_glyphMap.remove(k);

	delete m_activeGlyph;
	m_activeGlyph = 0;
	emit sendSelectGlyph(false);
}

bool GlyphScene::glyphExists(const int i)
{ return !(m_glyphMap.find(i) == nullptr); }

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
    m_connectionMap.clear();
}

QJsonObject GlyphScene::getGlyphProfile(int typeId)
{ return m_collector->element(typeId); }

void GlyphScene::mapConnection(GlyphConnection *x)
{
    const int &idx = x->connectionId();
    const int i = idx > -1 ? idx : getConnectionUid(x->hintId());
    m_connectionMap.insert(i, x);
    if(i != idx) x->setConnectionId(i);
}

void GlyphScene::unmapConnection(GlyphConnection *x)
{
    const int &idx = x->connectionId();
    GlyphConnectionPtr *y = m_connectionMap.find(idx);
    if(y) (*y)->setConnectionId(-1);
}

int GlyphScene::getConnectionUid(const int &base)
{
    int r = base + rand() & 127;
    bool vac = (m_connectionMap.find(r) == nullptr);
    while(!vac) {
        r += rand() & 127;
        vac = (m_connectionMap.find(r) == nullptr); 
    }
    return r;
}

sdb::L3DataIterator<int, GlyphConnection *, 128> GlyphScene::connectionsBegin() const
{ return m_connectionMap.begin(-1); }

sdb::L3DataIterator<int, GlyphItem *, 128> GlyphScene::glyphsBegin() const
{ return m_glyphMap.begin(-1); }

bool GlyphScene::connectNodes(const int &node0Id, const std::string &port0Name,
					const int &node1Id, const std::string &port1Name,
					const int &uid)
{
	GlyphItemPtr *n0 = m_glyphMap.find(node0Id);
	if(n0 == nullptr) {
		std::cout << "\n ERROR GlyphScene::connectNodes cannot find node0 " << node0Id;
		return false;
	}

	GlyphPort *pt0 = (*n0)->findPort(port0Name);
	if(pt0 == nullptr) {
		std::cout << "\n ERROR GlyphScene::connectNodes cannot find port0 " << port0Name;
		return false;
	}

	GlyphItemPtr *n1 = m_glyphMap.find(node1Id);
	if(n1 == nullptr) {
		std::cout << "\n ERROR GlyphScene::connectNodes cannot find node1 " << node1Id;
		return false;
	}

	GlyphPort *pt1 = (*n1)->findPort(port1Name);
	if(pt1 == nullptr) {
		std::cout << "\n ERROR GlyphScene::connectNodes cannot find port1 " << port1Name;
		return false;
	}
        
    GlyphConnection *conn = new GlyphConnection;
    conn->setConnectionId(uid);
	conn->originFrom(pt0);
    conn->destinationTo(pt1);
    addItem(conn);
    mapConnection(conn);

	return true;
}

void GlyphScene::preLoad()
{
	resetGlyphScene();
}

void GlyphScene::postLoad()
{}

} /// end of alo
