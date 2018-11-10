#ifndef ACA_MESH_LISTENER_OPS_H
#define ACA_MESH_LISTENER_OPS_H

#include "DrawableOps.h"
#include <math/SimpleBuffer.h>
#include <math/Vector3F.h>

QT_BEGIN_NAMESPACE
class QListWidget;
QT_END_NAMESPACE

namespace alo {
class AdaptableMesh;
class HistoryMesh;
class HistoryReform;
class JMesh;
class MeshListenerOps : public DrawableOps {
    
    std::map<std::string, JMesh> m_meshMap;
    std::string m_meshName;
    SimpleBuffer<Vector3F> posnml;
    SimpleBuffer<Vector3F> baryc;
    HistoryMesh *m_sourceMesh;
    HistoryReform *m_reformer;
    AdaptableMesh *m_mesh;
    unsigned m_upd;
    float m_lod;
    bool m_toRelocate;
    
public:
    MeshListenerOps();
    virtual ~MeshListenerOps();

    virtual void update() override;
    virtual void postUI() override;
 	virtual void addDrawableTo(DrawableScene *scene) override;

protected:
    
private:
    bool checkBroadcastTime();
    bool loadMesh(bool dataChanged);
    bool loadMeshMaster();
    void computeMesh();
    void setMeshDrawable(DrawableScene *scene);
    void listAvailableMeshes(QListWidget *wig);

};

}

#endif

