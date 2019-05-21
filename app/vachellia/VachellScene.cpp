/*
 *  VachellScene.cpp
 *  vachellia
 *
 *  interrupt render condition
 *  create/remove item
 *  visibility/attribute changed
 *  create/remove connection
 *  save/load
 *  
 *  2019/5/11
 */

#include "VachellScene.h"
#include "ops/allOps.h"
#include <qt_graph/GlyphOps.h>
#include <qt_graph/GlyphItem.h>
#include <qt_graph/GlyphConnection.h>
#include <math/CameraEvent.h>
#include <math/AFrustum.h>
#include <math/Hexahedron.h>
#include <interface/GlobalFence.h>
#include <boost/thread/lock_guard.hpp>
#include "io/NodeWriter.h"
#include <QApplication>
#include <QMessageBox>
#include <QProgressDialog>
#include <QDebug>

using namespace alo;

VachellScene::VachellScene(QObject *parent) : GlyphScene(parent)
{}

VachellScene::~VachellScene()
{}

GlyphOps *VachellScene::createOps(const QJsonObject &content)
{
    int k = content["id"].toInt();
    switch(k) {
        case ConeOps::Type :
            return new ConeOps;
        case CylinderOps::Type :
            return new CylinderOps;
        case SphereOps::Type :
            return new SphereOps;
        case BoxOps::Type :
            return new BoxOps;
        case HorizonOps::Type :
            return new HorizonOps;
        case VoxelOps::Type :
            return new VoxelOps;
        case RepeatOps::Type :
            return new RepeatOps;
        case UnionOps::Type :
            return new UnionOps;
        default:
            ;
    }
    return new GlyphOps; 
}

void VachellScene::postCreation(GlyphItem *item)
{
    interface::GlobalFence &fence = interface::GlobalFence::instance();
    boost::lock_guard<interface::GlobalFence> guard(fence);

	GlyphOps *op = item->ops();
	if(op->hasRenderable()) { 
		RenderableOps *dop = static_cast<RenderableOps *>(op);
		dop->addRenderableTo(this);
	}

    item->genToolTip();

    RenderableScene::setSceneChanged();
    emit sendUpdateScene();
}

void VachellScene::preDestruction(GlyphItem *item, const std::vector<GlyphConnection *> &connectionsToBreak)
{
    interface::GlobalFence &fence = interface::GlobalFence::instance();
    boost::lock_guard<interface::GlobalFence> guard(fence);

    foreach(GlyphConnection *conn, connectionsToBreak) {
        conn->breakUp();
    }

    GlyphOps *op = item->ops();
    if(op->hasRenderable()) { 
        RenderableOps *dop = static_cast<RenderableOps *>(op);
        dop->removeRenderableFromScene();
    }
    op->preDestruction();
    delete op;

    RenderableScene::setSceneChanged();
    emit sendUpdateScene();
}

void VachellScene::recvCameraChanged(const CameraEvent &x)
{
    GlyphDataType *block = firstGlyph();
    while(block) {
        for(int i=0;i<block->count();++i) {
            GlyphItem *g = block->value(i);
            GlyphOps *op = g->ops();
            op->recvCameraChanged(x);
        }
        block = nextGlyph(block);
    }
}

void VachellScene::recvRequestBound()
{
    GlyphOps *op = getActiveOps();
    if(!op) return;
    if(!op->hasDrawable() ) return;
    Hexahedron b;
    op->getDrawableBound(b);
    emit sendBound(b);
}

void VachellScene::onItemStateChanged(GlyphItem *item, QGraphicsItem *stateControlItem)
{
    interface::GlobalFence &fence = interface::GlobalFence::instance();
    boost::lock_guard<interface::GlobalFence> guard(fence);

    item->endEditState(stateControlItem);

    item->genToolTip();

	RenderableScene::setSceneChanged();
	emit sendUpdateScene();
}

void VachellScene::recvAttribChanged()
{
    interface::GlobalFence &fence = interface::GlobalFence::instance();
    boost::lock_guard<interface::GlobalFence> guard(fence);

    GlyphItem *item = getActiveGlyph();
    if(item) item->genToolTip();

    RenderableScene::setSceneChanged();
	emit sendUpdateScene();
}

void VachellScene::createConnection(GlyphConnection *conn, GlyphPort *port)
{
    if(!conn->canConnectTo(port) ) return;

    interface::GlobalFence &fence = interface::GlobalFence::instance();
    boost::lock_guard<interface::GlobalFence> guard(fence);
	
	conn->destinationTo(port);
    RenderableScene::setSceneChanged();
	emit sendUpdateScene();
}

void VachellScene::removeConnection(GlyphConnection *conn)
{
    interface::GlobalFence &fence = interface::GlobalFence::instance();
    boost::lock_guard<interface::GlobalFence> guard(fence);

	conn->breakUp();
    RenderableScene::setSceneChanged();
	emit sendUpdateScene();
}

AFileDlgProfile VachellScene::SWriteProfile(AFileDlgProfile::FWrite,
        "Choose File To Save",
        ":images/save_big.png",
        "Save Vachillia Scene",
        "Save .hes",
        ".hes",
        "./",
        "untitled");

bool VachellScene::save()
{
    interface::GlobalFence &fence = interface::GlobalFence::instance();
    boost::lock_guard<interface::GlobalFence> guard(fence);

    AFileDlg d(SWriteProfile, QApplication::activeWindow() );
    int res = d.exec();
    if(res == QDialog::Rejected) {
        qDebug()<<" abort ";
        return false;
    }
    
    const std::string saveFileName = SWriteProfile.getFilePath();
    H5GraphIO hio;
	bool stat = hio.begin(saveFileName, HDocument::oCreate );
	if(!stat) {
        QMessageBox msgBox;
        msgBox.setText(QString("Cannot save to file %1").arg(QString::fromStdString(saveFileName)));
        msgBox.exec();
        return false;
    }
    
    //QApplication::setOverrideCursor(Qt::WaitCursor);
    
    QProgressDialog progress("Saving...", QString(), 0, 1, QApplication::activeWindow() );
    progress.setWindowModality(Qt::ApplicationModal);
    progress.show();
    
    hio.sceneBegin();

    vchl::NodeWriter nodeWriter;
    
    GlyphDataType *block = firstGlyph();
    while(block) {
        for (int i=0;i<block->count();++i) { 
            const GlyphItem *ci = block->value(i);
            hio.nodeBegin(ci->glyphId());

            QPointF pos = ci->mapToScene(QPointF());
            hio.writeNodePosition(pos.x(), pos.y());

            const GlyphOps *ops = ci->getOps();
            
            nodeWriter.write(hio, ops);

            hio.nodeEnd();
        }
        
        block = nextGlyph(block);
    }
	
    hio.sceneEnd();
    
    progress.setValue(1);
    
    hio.end();
    
    //QApplication::restoreOverrideCursor();
    return true;
}

AFileDlgProfile VachellScene::SReadProfile(AFileDlgProfile::FRead,
        "Choose File To Open",
        ":images/open_big.png",
        "Open Vachillia Scene",
        "Open .hes",
        ".hes",
        "./",
        "");

bool VachellScene::load()
{
    interface::GlobalFence &fence = interface::GlobalFence::instance();
    boost::lock_guard<interface::GlobalFence> guard(fence);

/// todo loss of data confirmation
    AFileDlg d(SReadProfile, QApplication::activeWindow() );
    int res = d.exec();
    if(res == QDialog::Rejected) {
        qDebug()<<" abort ";
        return false;
    }
    
    QApplication::setOverrideCursor(Qt::WaitCursor);
    
    qDebug() << " todo load from file " << QString::fromStdString(SReadProfile.getFilePath());
    
    QApplication::restoreOverrideCursor();
    return true;
}
