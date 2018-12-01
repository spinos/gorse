#include "MeshListenerOps.h"
#include <QListWidget>
#include <qt_ogl/DrawableScene.h>
#include <qt_ogl/DrawableObject.h>
#include <qt_ogl/DrawableResource.h>
#include <boost/interprocess/mapped_region.hpp>
#include <ipc/SharedMemoryObject.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <jsn/JMesh.h>
#include <mesh/EdgeCollapse.h>
#include <mesh/HistoryMesh.h>
#include <mesh/HistoryReformSrc.h>
#include <qt_base/AFileDlg.h>
#include <h5/V1H5IO.h>
#include <h5/V1HBase.h>
#include <h5_mesh/HHistoryMesh.h>

using namespace boost::interprocess;
using namespace boost::property_tree;
    
namespace alo {

AFileDlgProfile MeshListenerOps::SWriteProfile(AFileDlgProfile::FWrite,
        "Choose File To Save",
        ":images/save_big.png",
        "Save mesh",
        "Save .hes",
        ".hes",
        "./",
        "untitled");
   
MeshListenerOps::MeshListenerOps() : m_upd(0), 
m_meshName("unknown"),
m_lod(.5f),
m_shoUV(false)
{ 
    m_mesh = new AdaptableMesh; 
    m_stageMesh = new HistoryMesh;
    m_stageMesh->createTriangleMesh(1024, 1024);
    m_stageMesh->addHistoryStage();
    m_sourceMesh = new HistoryMesh;
    m_reformer = new HistoryReformSrc;
    DrawableResource *rec = createResource();
    setResource(rec);
}

MeshListenerOps::~MeshListenerOps()
{ 
    delete m_mesh; 
    delete m_stageMesh;
    delete m_sourceMesh;
    delete m_reformer;
}
    
void MeshListenerOps::update()
{
    QAttrib * ameshes = findAttrib("ipc_mesh_list");
    ListAttrib *fmeshes = static_cast<ListAttrib *>(ameshes);
    std::string meshName;
    fmeshes->getValue(meshName);
    
    const bool meshSelectionChanged = (m_meshName != meshName);
    m_meshName = meshName;
    
    const bool dataChanged = checkBroadcastTime();
    
    if( dataChanged || meshSelectionChanged)
        loadMesh(dataChanged);
        
    QAttrib * alod = findAttrib("lod");
    FloatAttrib *flod = static_cast<FloatAttrib *>(alod);
    flod->getValue(m_lod);
    
    QAttrib * ashouv = findAttrib("sho_uv");
    BoolAttrib *fshouv = static_cast<BoolAttrib *>(ashouv);
    fshouv->getValue(m_shoUV);

    computeMesh();

    DrawableScene *scene = drawableScene();

    DrawableResource *rec = resource();
    DrawableObject *d = rec->drawable();

    if(rec->toRelocate() || dataChanged || meshSelectionChanged) {
        scene->enqueueRemoveDrawable(d->drawId(), opsId());
        DrawableObject *d1 = createDrawable();
        rec->attachToDrawable(d1, m_mesh->numIndices());
        
        scene->lock();
        scene->enqueueCreateDrawable(d1, opsId());
        scene->unlock();
        
    } else {
        d->setDrawArrayLength(m_mesh->numIndices());
        scene->enqueueEditDrawable(d->drawId(), opsId());
    }
}

void MeshListenerOps::postUI()
{
    QAttrib * ameshes = findAttrib("ipc_mesh_list");
    ListAttrib *fmeshes = static_cast<ListAttrib *>(ameshes);
    QListWidget *lmeshes = fmeshes->widget();

    listAvailableMeshes(lmeshes);
}

void MeshListenerOps::listAvailableMeshes(QListWidget *wig)
{
    m_meshMap.clear();
    m_meshName = "unknown";
    
    wig->clear();

    SharedMemoryObject shm;
    mapped_region region = shm.readRegion("MySharedMemory");
    if(region.get_size() < 1) return;

    const char *mem = static_cast<const char*>(region.get_address());
    std::stringstream sst(mem);
    std::cout<<" "<<sst.str();
    
    ptree pt;
    json_parser::read_json(sst, pt);
    BOOST_FOREACH(ptree::value_type const&v, pt.get_child("meshes")) {

        std::string meshName = v.second.get<std::string>("name");
        m_meshMap[meshName] = JMesh();
        m_meshMap[meshName].readPtree(v.second);
        
        wig->addItem(QString(meshName.c_str()) );
    }
}

void MeshListenerOps::addDrawableTo(DrawableScene *scene)
{ 
    computeMesh();
    setDrawableScene(scene);
    DrawableResource *rec = resource();
    DrawableObject *d = createDrawable();
    rec->attachToDrawable(d, m_mesh->numIndices());

    scene->lock();
    scene->enqueueCreateDrawable(d, opsId());
    scene->unlock();
}

bool MeshListenerOps::checkBroadcastTime()
{
    SharedMemoryObject shm;
    mapped_region region = shm.readRegion("MySharedMemory");
    if(region.get_size() < 1) return false;

    const char *mem = static_cast<const char*>(region.get_address());
    std::stringstream sst(mem);
    
    ptree pt;
    json_parser::read_json(sst, pt);

    unsigned upd = pt.get<unsigned>("updated");
/// outdated
    bool changed = m_upd < upd;
    m_upd = upd;
    return changed;
}

bool MeshListenerOps::loadMeshMaster()
{
    SharedMemoryObject shm;
    mapped_region region = shm.readRegion("MySharedMemory");
    if(region.get_size() < 1) return false;

    const char *mem = static_cast<const char*>(region.get_address());
    std::stringstream sst(mem);

    ptree pt;
    json_parser::read_json(sst, pt);
    BOOST_FOREACH(ptree::value_type const&v, pt.get_child("meshes")) {

        std::string meshName = v.second.get<std::string>("name");
        if(meshName == m_meshName) {
            m_meshMap[meshName].readPtree(v.second);
            return true;
        }
    }
    return false;
}

bool MeshListenerOps::loadMesh(bool dataChanged)
{
    if(dataChanged) {
        if(!loadMeshMaster())
            return false;
    }
    
    SharedMemoryObject shm;
    mapped_region region = shm.readRegion(m_meshName.c_str());
    if(region.get_size() < 1) return false;
    
    const JMesh &msh = m_meshMap[m_meshName];
        
    const int &nv = msh.nv();
    const int &nt = msh.nt();
    m_sourceMesh->createTriangleMesh(nv, nt);
    
    const char *mem = static_cast<const char*>(region.get_address());
    const Vector3F *pos = (const Vector3F *)&mem[msh.getPosLoc()];
    m_sourceMesh->copyPositionsFrom(pos);
    const unsigned *ind = (const unsigned *)&mem[msh.getIndLoc()];
    m_sourceMesh->copyIndicesFrom(ind);
    
    m_sourceMesh->clearUVSets();

    std::vector<JMesh::UVSet>::const_iterator uvit = msh.uvSetBegin();
    for(;uvit!=msh.uvSetEnd();++uvit) {
        Float2 *uvd = m_sourceMesh->addUVSet(uvit->_name);
        const Float2 *uvs = (const Float2 *)&mem[uvit->_uv];
        memcpy(uvd, uvs, nt * 24);
    }

    m_sourceMesh->calculateVertexNormals();
    
    EdgeCollapse ech;
    ech.simplify(m_sourceMesh);
    
    return true;
}

void MeshListenerOps::computeMesh()
{
    if(m_meshName == "unknown")
        m_mesh->createMinimal();
    else
        m_reformer->reformSrc(m_mesh, m_stageMesh, m_lod, m_sourceMesh);

    DrawableResource *rec = resource();
    updateMeshResouce(rec, m_mesh, m_shoUV);
}

bool MeshListenerOps::hasMenu() const
{
    if(m_meshName == "unknown") return false;
    return true; 
}

void MeshListenerOps::getMenuItems(std::vector<std::pair<std::string, int > > &ks) const 
{
    ks.push_back(std::make_pair("Save", AFileDlgProfile::FWrite));
}

void MeshListenerOps::recvAction(int x) 
{
    if(x == AFileDlgProfile::FWrite) saveToFile(SWriteProfile.getFilePath());
}

AFileDlgProfile *MeshListenerOps::writeFileProfileR () const
{
    SWriteProfile._notice = boost::str(boost::format("level-of-detail mesh cache \nn vertices [%1%:%2%] \nn faces [%3%:%4%] ") 
        % m_sourceMesh->minNumVertices() % m_sourceMesh->maxNumVertices() 
        % m_sourceMesh->minNumTriangles() % m_sourceMesh->maxNumTriangles() );
    return &SWriteProfile; 
}

bool MeshListenerOps::saveToFile(const std::string &fileName)
{
    ver1::H5IO hio;
    bool stat = hio.begin(fileName, HDocument::oCreate);
    if(!stat) return false;

    ver1::HBase w("/world");
    ver1::HBase b("/world/meshes");

    HHistoryMesh hmh("/world/meshes/" + m_meshName);
    hmh.save(m_sourceMesh);
    hmh.close();

    b.close();
    w.close();

    hio.end();
    return true; 
}

}
