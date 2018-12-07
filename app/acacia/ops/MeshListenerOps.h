#ifndef ACA_MESH_LISTENER_OPS_H
#define ACA_MESH_LISTENER_OPS_H

#include "GranulateReduce.h"

QT_BEGIN_NAMESPACE
class QListWidget;
QT_END_NAMESPACE

namespace alo {
class AdaptableMesh;
class HistoryMesh;
class JMesh;
class MeshListenerOps : public GranulateReduce {
    
    std::map<std::string, JMesh> m_meshMap;
    std::string m_meshName;
    unsigned m_upd;
    float m_lod;
    bool m_shoUV;
    
public:
    MeshListenerOps();
    virtual ~MeshListenerOps();

    virtual void update() override;
    virtual void postUI() override;
 	virtual void addDrawableTo(DrawableScene *scene) override;
    virtual bool hasMenu() const override;

protected:

    virtual std::string meshCacheName() const override;
    
private:
    bool checkBroadcastTime();
    bool loadMesh(bool dataChanged);
    bool loadMeshMaster();
    void computeMesh();
    void listAvailableMeshes(QListWidget *wig);
    
};

}

#endif

