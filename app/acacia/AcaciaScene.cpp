/*
 *  AcaciaScene.cpp
 *  acacia
 *
 *  2019/7/11
 */

#include "AcaciaScene.h"
#include "ops/AllOps.h"
#include <qt_graph/GlyphItem.h>
#include <qt_graph/GlyphConnection.h>
#include <math/CameraEvent.h>
#include <math/AFrustum.h>

using namespace alo;

AcaciaScene::AcaciaScene(QObject *parent)
    : GlyphScene(parent)
{}

AcaciaScene::~AcaciaScene()
{}

GlyphOps *AcaciaScene::createOps(const QJsonObject &content)
{ 
    int k = content["id"].toInt();
    switch(k) {
        case ThinPlateTest::Type :
            return new ThinPlateTest;
        case LeafTest::Type :
            return new LeafTest;
        case BranchTest::Type :
            return new BranchTest;
        case MeshFusionTest::Type :
            return new MeshFusionTest;
        case SsdfTest::Type :
            return new SsdfTest;
    case AcaciaOpsType::AoSphere :
        return new SphereOps;
    case AcaciaOpsType::AoRoot :
        return new RootOps;
    case AcaciaOpsType::AoLodMesh :
        return new LodMeshOps;
    case AcaciaOpsType::AoStaticMesh :
        return new SimpleMeshOps;
    case AcaciaOpsType::AoEdgeCollapseTest :
        return new EdgeCollapseTest;
    case AcaciaOpsType::AoLodMeshIn :
        return new LodMeshInOps;
    case AcaciaOpsType::AoPVSTest :
        return new PVSTest;
    case AcaciaOpsType::AoInstancedTest :
        return new InstancedTest;
    case AcaciaOpsType::AoGeodesicDistanceTest :
        return new GeodesicDistanceTest;
    default:
        break;
    }
    return new GlyphOps; 
}

void AcaciaScene::postCreation(GlyphItem *item)
{
	GlyphOps *op = item->ops();
	if(op->hasDrawable()) { 
		DrawableOps *dop = static_cast<DrawableOps *>(op);
		dop->addDrawableTo(this);
	}
    emit sendUpdateDrawable();
}

void AcaciaScene::postCreationBlocked(GlyphItem *item)
{}

void AcaciaScene::preDestruction(GlyphItem *item, const std::vector<GlyphConnection *> &connectionsToBreak)
{
    GlyphOps *op = item->ops();
    if(op->hasDrawable()) { 
        DrawableOps *dop = static_cast<DrawableOps *>(op);
        dop->removeDrawableFromScene();
    }
    delete op;
    emit sendUpdateDrawable();
}

void AcaciaScene::recvCameraChanged(const CameraEvent &x)
{
    sdb::L3DataIterator<int, GlyphItem *, 128> iter = glyphsBegin();
    for(;!iter.done();iter.next()) {

        GlyphItem *g = *iter.second;
            GlyphOps *op = g->ops();
            op->recvCameraChanged(x);
    }
}

void AcaciaScene::recvRequestBound()
{
    GlyphOps *op = getActiveOps();
    if(!op) return;
    if(!op->hasDrawable() ) return;
    Hexahedron b;
    op->getDrawableBound(b);
    emit sendBound(b);
}

void AcaciaScene::onItemStateChanged(GlyphItem *item, QGraphicsItem *stateControlItem)
{ 
    item->endEditState(stateControlItem);
    emit sendUpdateDrawable(); 
}

bool AcaciaScene::makeConnection(GlyphConnection *conn, GlyphPort *port)
{
    if(!conn->canConnectTo(port) ) return false;
	
	conn->destinationTo(port);
    GlyphScene::mapConnection(conn);

    emit sendUpdateDrawable();
    return true;
}

void AcaciaScene::removeConnection(GlyphConnection *conn)
{
	conn->breakUp();
    GlyphScene::unmapConnection(conn);
    
    emit sendUpdateDrawable();
}

