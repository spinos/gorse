#ifndef ACA_EDGE_COLLAPSE_TEST_H
#define ACA_EDGE_COLLAPSE_TEST_H

#include "DrawableOps.h"

namespace alo {
class AdaptableMesh;
class HistoryMesh;
class HistoryReformSrc;
class EdgeCollapseTest : public DrawableOps {
    
    AdaptableMesh *m_mesh;
    HistoryMesh *m_stageMesh;
    HistoryMesh *m_sourceMesh;
    HistoryReformSrc *m_reformer;
    float m_lod;
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
    
};

}

#endif

