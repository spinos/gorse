/*
 *  VachellScene.cpp
 *  vachellia
 *  
 *  2019/5/24
 */

#include "VachellScene.h"
#include "ops/allOps.h"
#include <qt_graph/GlyphOps.h>
#include <qt_graph/GlyphItem.h>
#include <qt_graph/GlyphConnection.h>
#include <qt_graph/GlyphPort.h>
#include <math/Hexahedron.h>
#include <interface/GlobalFence.h>
#include <boost/thread/lock_guard.hpp>
#include "io/NodeWriter.h"
#include "io/NodeReader.h"
#include "h5_graph/HGraphConnectionWriter.h"
#include "h5_graph/HGraphConnectionReader.h"
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
        case TrunkOps::Type :
            return new TrunkOps;
        case TerrainOps::Type :
            return new TerrainOps;
        case CanopyOps::Type :
            return new CanopyOps;
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

	postCreationBlocked(item);

    RenderableScene::setSceneChanged();
    emit sendUpdateScene();
}

void VachellScene::postCreationBlocked(GlyphItem *item)
{
    GlyphOps *op = item->ops();
    if(op->hasRenderable()) { 
        RenderableOps *dop = static_cast<RenderableOps *>(op);
        dop->addRenderableTo(this);
    }

    item->genToolTip();
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
    sdb::L3DataIterator<int, GlyphItem *, 128> iter = glyphsBegin();
    for(;!iter.done();iter.next()) {

        GlyphItem *g = *iter.second;
            GlyphOps *op = g->ops();
            op->recvCameraChanged(x);

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

	RenderableScene::setSceneChanged();
	emit sendUpdateScene();
}

void VachellScene::recvAttribChanged()
{
    interface::GlobalFence &fence = interface::GlobalFence::instance();
    boost::lock_guard<interface::GlobalFence> guard(fence);

    GlyphItem *item = getActiveGlyph();
    if(item) item->updateOps();

    RenderableScene::setSceneChanged();
	emit sendUpdateScene();
}

bool VachellScene::makeConnection(GlyphConnection *conn, GlyphPort *port)
{
    if(!conn->canConnectTo(port) ) return false;

    interface::GlobalFence &fence = interface::GlobalFence::instance();
    boost::lock_guard<interface::GlobalFence> guard(fence);
	
	conn->destinationTo(port);
    GlyphScene::mapConnection(conn);
    
    RenderableScene::setSceneChanged();
	emit sendUpdateScene();
    return true;
}

void VachellScene::removeConnection(GlyphConnection *conn)
{
    interface::GlobalFence &fence = interface::GlobalFence::instance();
    boost::lock_guard<interface::GlobalFence> guard(fence);

	conn->breakUp();
    GlyphScene::unmapConnection(conn);
    
    RenderableScene::setSceneChanged();
	emit sendUpdateScene();
}

bool VachellScene::cleanSlate()
{
    interface::GlobalFence &fence = interface::GlobalFence::instance();
    boost::lock_guard<interface::GlobalFence> guard(fence);

    if(numChanges() > 1) {
        QMessageBox msgBox;
        msgBox.setText("The current scene has been modified.");
        msgBox.setInformativeText("Do you want to create a new one thus discard all changes?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();
        if(ret == QMessageBox::Cancel) {
            return false;
        }
    }
    
    GlyphScene::preLoad();
    RenderableScene::resetRenderableScene();

    GlyphScene::postLoad();
    RenderableScene::setSceneChanged();
    emit sendUpdateScene();
    return true;
}

AFileDlgProfile VachellScene::SWriteProfile(AFileDlgProfile::FWrite,
        "Choose File To Save",
        ":images/save_big.png",
        "Save Vachillia Scene",
        "Save .hes",
        ".hes",
        "./",
        "untitled");

bool VachellScene::save(bool doChooseFile)
{
    interface::GlobalFence &fence = interface::GlobalFence::instance();
    boost::lock_guard<interface::GlobalFence> guard(fence);

    if(!doChooseFile && numChanges() < 2) {
        QMessageBox msgBox;
        msgBox.setText("The current scene has not been modified.");
        msgBox.setInformativeText("No need to save.");
        msgBox.exec();
        return false;
    }

    std::string saveFileName = renderableSceneName();
    if(saveFileName == "unknown" || doChooseFile) {
        AFileDlg d(SWriteProfile, QApplication::activeWindow() );
        int res = d.exec();
        if(res == QDialog::Rejected) {
            qDebug()<<" abort ";
        } else {
            saveFileName = SWriteProfile.getFilePath();
        }
    }

    if(saveFileName == "unknown") return false;
    
    H5GraphIO hio;
	bool stat = hio.begin(saveFileName, HDocument::oCreate );
	if(!stat) {
        QMessageBox msgBox;
        msgBox.setText(QString("Cannot save to file %1").arg(QString::fromStdString(saveFileName)));
        msgBox.exec();
        return false;
    }
    
    QProgressDialog progress("Saving...", QString(), 0, 1, QApplication::activeWindow() );
    progress.setWindowModality(Qt::ApplicationModal);
    progress.show();
    
    hio.sceneBegin();

    vchl::NodeWriter nWriter;
    
    sdb::L3DataIterator<int, GlyphItem *, 128> glyphIter = glyphsBegin();
    for(;!glyphIter.done();glyphIter.next()) {

        GlyphItem *ci = *glyphIter.second;
            GlyphOps *ops = ci->ops();
            ops->preSave();

            QJsonObject content = getGlyphProfile(ops->opsTypeId());

            hio.nodeBegin(ci->glyphId());

            QPointF pos = ci->mapToScene(QPointF());
            hio.writeNodePosition(pos.x(), pos.y());
           
            nWriter.write(hio, ops, content);

            hio.nodeEnd();
    }

    HGraphConnectionWriter cWriter;
    cWriter.writeConnections(hio, connectionsBegin());
	
    hio.sceneEnd();
    
    hio.end();

    progress.setValue(1);
    
    resetChangeCount();
    RenderableScene::setName(saveFileName);
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

bool VachellScene::open()
{
    interface::GlobalFence &fence = interface::GlobalFence::instance();
    boost::lock_guard<interface::GlobalFence> guard(fence);

    if(numChanges() > 1) {
        QMessageBox msgBox;
        msgBox.setText("The current scene has been modified.");
        msgBox.setInformativeText("Do you want to open another one thus discard all changes?");
        msgBox.setStandardButtons(QMessageBox::Open | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();
        if(ret == QMessageBox::Cancel) {
            return false;
        }
    }

    AFileDlg d(SReadProfile, QApplication::activeWindow() );
    int res = d.exec();
    if(res == QDialog::Rejected) {
        return false;
    }
    
    const std::string openFileName = SReadProfile.getFilePath();
    H5GraphIO hio;
    bool stat = hio.begin(openFileName, HDocument::oReadOnly );
    if(!stat) {
        QMessageBox msgBox;
        msgBox.setText(QString("Cannot open file %1").arg(QString::fromStdString(openFileName)));
        msgBox.exec();
        return false;
    }

    QProgressDialog progress("Opening...", QString(), 0, 4, QApplication::activeWindow() );
    progress.setWindowModality(Qt::ApplicationModal);
    progress.show();

    GlyphScene::preLoad();
    RenderableScene::resetRenderableScene();
    RenderableScene::beginProgress();

    progress.setValue(1);

    stat = hio.openScene();

    if(stat) {

        std::vector<std::string> nodeNames;
        hio.lsNodes(nodeNames);

        vchl::NodeReader reader;

        std::vector<std::string>::const_iterator it = nodeNames.begin();
        for(;it!=nodeNames.end();++it) {
            hio.openNode(*it);

            reader.readNode(hio);

            GlyphScene::CreateGlyphParameter prof;
            prof._type = reader.nodeTypeId();
            prof._id = reader.nodeUid();
            const float *pos = reader.nodePosition();
            prof._pos = QPointF(pos[0], pos[1]);
            prof._isLoading = true;
        
            GlyphItem *g = GlyphScene::createGlyph(prof);

            QJsonObject content = getGlyphProfile(prof._type);
            GlyphOps *ops = prof._ops;
            reader.read(hio, ops, content);

            g->setPostLoadVisibleState(reader.isVisible());
            g->setPostLoadActivatedState(reader.isActivated());

            hio.closeNode();
        }

        progress.setValue(2);

        std::vector<std::string> connectionNames;
        hio.lsConnections(connectionNames);

        HGraphConnectionReader cReader;

        it = connectionNames.begin();
        for(;it!=connectionNames.end();++it) {
            hio.openConnection(*it);

            cReader.readConnection(hio);

            connectNodes(cReader.node0Id(), cReader.port0Name(), 
                        cReader.node1Id(), cReader.port1Name(),
                        cReader.connectionId());

            hio.closeConnection();
        }

        hio.closeScene();

        progress.setValue(3);

        sdb::L3DataIterator<int, GlyphItem *, 128> glyphIter = glyphsBegin();
        for(;!glyphIter.done();glyphIter.next()) {
            GlyphItem *gi = *glyphIter.second;
            gi->postLoad();
        }

    }

    hio.end();

    progress.setValue(4);
    RenderableScene::endProgress();

    if(!stat) {
        QMessageBox msgBox;
        msgBox.setText(QString("Not a vachelloa scene file %1").arg(QString::fromStdString(openFileName)));
        msgBox.exec();
        return false;
    }

    GlyphScene::postLoad();
    RenderableScene::setName(openFileName);
    RenderableScene::setSceneChanged();
    emit sendUpdateScene();
    return true;
}
