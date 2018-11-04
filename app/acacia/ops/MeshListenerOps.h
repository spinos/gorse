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
class MeshListenerOps : public DrawableOps {
    
    SimpleBuffer<Vector3F> posnml;
    SimpleBuffer<Vector3F> baryc;
    AdaptableMesh *m_mesh;
    bool m_toRelocate;
    
public:
    MeshListenerOps();
    virtual ~MeshListenerOps();

    virtual void update() override;
    virtual void postUI() override;
 	virtual void addDrawableTo(DrawableScene *scene) override;

protected:
    
private:
    void computeMesh();
    void setMeshDrawable(DrawableScene *scene);
    void listAvailableMeshes(QListWidget *wig);

};

}

#endif

