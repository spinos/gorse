/*
 *  VachellScene.h
 *  vachellia
 *
 *  2019/5/11
 */

#ifndef VCHL_VACHELL_SCENE_H
#define VCHL_VACHELL_SCENE_H

#include <qt_graph/GlyphScene.h>
#include <interface/RenderableScene.h>

namespace alo {
class GlyphOps;
class CameraEvent;
class Hexahedron;
}

class VachellScene : public alo::GlyphScene, public alo::RenderableScene
{
    Q_OBJECT

public:
	VachellScene(QObject *parent = 0);
	virtual ~VachellScene();
	
	virtual void onItemStateChanged(alo::GlyphItem *item, QGraphicsItem *stateControlItem) override;
    virtual void createConnection(alo::GlyphConnection *conn, alo::GlyphPort *port) override;
    virtual void removeConnection(alo::GlyphConnection *conn) override;
	
signals:
	void sendStopRender();
	void sendUpdateScene();
	void sendBound(const alo::Hexahedron &x);

public slots:
    void recvAttribChanged();
	void recvCameraChanged(const alo::CameraEvent &x);
	void recvRequestBound();

protected:
    virtual alo::GlyphOps *createOps(const QJsonObject &content) override;
    virtual void postCreation(alo::GlyphItem *item) override;
    virtual void preDestruction(alo::GlyphItem *item, const std::vector<alo::GlyphConnection *> &connectionsToBreak) override;
	virtual int getUid(const int typeId) override;
	
private:

};

#endif