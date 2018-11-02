#ifndef ACA_EDGE_COLLAPSE_TEST_H
#define ACA_EDGE_COLLAPSE_TEST_H

#include "DrawableOps.h"
#include <math/SimpleBuffer.h>
#include <math/Vector3F.h>

namespace alo {
class AdaptableMesh;
class HistoryMesh;
class HistoryReform;
class EdgeCollapseTest : public DrawableOps {
    
    SimpleBuffer<Vector3F> posnml;
    SimpleBuffer<Vector3F> baryc;
    AdaptableMesh *m_mesh;
    HistoryMesh *m_sourceMesh;
    HistoryReform *m_reformer;
    float m_lod;
    bool m_toRelocate;
    
public:
    EdgeCollapseTest();
    virtual ~EdgeCollapseTest();

    virtual void update() override;

 	virtual void addDrawableTo(DrawableScene *scene) override;

protected:
    
private:
    void computeMesh();
    void setMeshDrawable(DrawableScene *scene);
    
};

}

#endif

