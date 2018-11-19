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
    static AFileDlgProfile SWriteProfile;
    
public:
    EdgeCollapseTest();
    virtual ~EdgeCollapseTest();

    virtual void update() override;

 	virtual void addDrawableTo(DrawableScene *scene) override;
    virtual bool hasMenu() const override;
    virtual void getMenuItems(std::vector<std::pair<std::string, int > > &ks) const override;
    virtual void recvAction(int x) override;
    virtual AFileDlgProfile *writeFileProfileR () const override;

protected:
    
private:
    void computeMesh();
    bool saveToFile(const std::string &fileName);
    void setMeshDrawable(DrawableScene *scene);
    
};

}

#endif

