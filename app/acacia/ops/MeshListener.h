/*
 *  MeshListener.h
 *  acacia
 *
 */

#ifndef ACA_MESH_LISTENER_H
#define ACA_MESH_LISTENER_H

#include "GranulateReduce.h"

QT_BEGIN_NAMESPACE
class QListWidget;
QT_END_NAMESPACE

namespace alo {
class JMesh;
class AdaptableMesh;
class MeshListener : public GranulateReduce {
    
    std::map<std::string, JMesh> m_meshMap;
    std::string m_meshName;
    unsigned m_upd;
    
public:
    MeshListener();
    virtual ~MeshListener();

    virtual void postUI() override;
 	virtual void addDrawableTo(DrawableScene *scene) override;
    virtual bool hasMenu() const override;

protected:
    bool checkMeshChange(bool &dataChanged);
    virtual std::string meshCacheName() const override;
    bool loadMesh(AdaptableMesh *transient, bool dataChanged);
    
private:
    bool checkBroadcastTime();
    bool loadMeshMaster();
    void listAvailableMeshes(QListWidget *wig);
    
};

}

#endif

