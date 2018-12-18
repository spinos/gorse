/*
 *  GranulateReduce.h
 *  acacia
 *
 *  many part reduction, reform, and caching 
 *
 */
 
#ifndef ACA_GRANULATE_REDUCE_H
#define ACA_GRANULATE_REDUCE_H

#include "DrawableOps.h"

namespace alo {
class AdaptableMesh;
class HistoryMesh;
class ViewFrustumCull;
class VisibleDetail;
class LevelOfDetailSelect;
class Hexahedron;
class GranulateReduce : public DrawableOps {

    static AFileDlgProfile SWriteProfile;
    int m_nv0, m_nv1, m_nt0, m_nt1;

public:
    GranulateReduce();
    virtual ~GranulateReduce();

    virtual bool hasMenu() const override;
    virtual void getMenuItems(std::vector<std::pair<std::string, int > > &ks) const override;
    virtual void recvAction(int x) override;
    virtual AFileDlgProfile *writeFileProfileR () const override;
    
protected:
    int reduce(ViewFrustumCull *culler, const AdaptableMesh *srcMesh);
    void viewDependentReform(const ViewFrustumCull *culler, VisibleDetail *details);
    void simpleReform(const float &lod, bool shoAsUV);
    
    struct MeshReformTrio {
		AdaptableMesh *_outMesh;
		HistoryMesh *_stageMesh;
        HistoryMesh *_srcMesh;
	};
	
	void addMeshReformTrio();
	
	int numMeshTrios() const;
	MeshReformTrio &meshTrio(int i);

    virtual std::string meshCacheName() const;
	
	const int &outMeshNv(int i) const;
	static void SimplifyAndReform(MeshReformTrio &p, DrawableResource *rec);
    static void Reform(MeshReformTrio &p, int nv, bool forcedUpdate, DrawableResource *rec);
    static void LodReform1(const float &lod, bool shoAsUV,
                MeshReformTrio &p, DrawableResource *rec);

private:
    bool saveToFile(const std::string &fileName);
    
    std::vector<MeshReformTrio> m_meshes;
	
};

}

#endif

