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

namespace alo {

GlyphScene::GlyphScene(QObject *parent)
    : QGraphicsScene(parent)//, 
//m_lastSelectedGlyph(NULL)
{}

GlyphScene::~GlyphScene()
{}

void GlyphScene::createGlyph(const QPixmap &pix, int typ, const QPointF & pos)
{
	qDebug()<<"GlyphScene::createGlyph"<< pix << " id " << typ;
	GlyphItem * g = new GlyphItem(pix, typ);
	addItem(g);

	g->setPos(pos);

	GlyphHalo* hal = new GlyphHalo;
	QPointF posmts = pos + g->localCenter();
	hal->setPos(posmts.x() - 50, posmts.y() - 50 );
	addItem(hal);
	g->setHalo(hal);

	buildItem(g);
}

void GlyphScene::buildItem(GlyphItem *item)
{}

/*
	const int & gtype = pieceTypGrp.x();
	const int & ggroup = pieceTypGrp.y();
	
	GlyphBuilder bdr;
	bdr.build(g, gtype, ggroup);
	

GardenGlyph * GlyphScene::getGround()
{	
/// search selected first
	foreach(GardenGlyph* its_, m_selectedGlyph) {
		GardenGlyph *g = its_;
		PieceAttrib* pa = g->attrib();
		if(pa->isGround() )
			return checkGroundConnection(g);
	}
	
	foreach(QGraphicsItem *its_, items()) {
		
		if(its_->type() == GardenGlyph::Type) {
			GardenGlyph *g = (GardenGlyph*) its_;
			PieceAttrib* pa = g->attrib();
			if(pa->isGround() )
				return checkGroundConnection(g);
		}
	}
	qDebug()<<"  ERROR cannot find ground to grow on";
	return NULL;
}

void GlyphScene::selectGlyph(GardenGlyph* gl)
{
	if(!m_selectedGlyph.contains(gl) )
		m_selectedGlyph<<gl; 
	m_lastSelectedGlyph = gl;
	emit sendSelectGlyph(true);
}

void GlyphScene::deselectGlyph()
{
	foreach(GardenGlyph * gl, m_selectedGlyph) {
		gl->hideHalo();
	}
	m_selectedGlyph.clear();
	m_lastSelectedGlyph = NULL;
	emit sendSelectGlyph(false);
}

GardenGlyph* GlyphScene::lastSelectedGlyph()
{ return m_lastSelectedGlyph; }

const GardenGlyph* GlyphScene::lastSelectedGlyph() const
{ return m_lastSelectedGlyph; }

const ATriangleMesh* GlyphScene::lastSelectedGeom() const
{
	if(!m_lastSelectedGlyph) 
		return NULL;
	PieceAttrib* attr = m_lastSelectedGlyph->attrib();
	if(!attr->hasGeom())
		return NULL;
	
	gar::SelectProfile selprof;
	return attr->selectGeom(&selprof);
}*/

}
