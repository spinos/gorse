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
#include <mesh/HistoryMesh.h>
#include <mesh/HistoryReformSrc.h>

using namespace boost::interprocess;
using namespace boost::property_tree;
    
namespace alo {

MeshListenerOps::MeshListenerOps() : m_upd(0), 
m_meshName("unknown"),
m_lod(.5f),
m_shoUV(false)
{}

MeshListenerOps::~MeshListenerOps()
{}
    
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
    setDrawableScene(scene);
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
    
    AdaptableMesh transient;
    transient.createTriangleMesh(nv, nt);
    
    const char *mem = static_cast<const char*>(region.get_address());
    const Vector3F *pos = (const Vector3F *)&mem[msh.getPosLoc()];
    transient.copyPositionsFrom(pos);
    const unsigned *ind = (const unsigned *)&mem[msh.getIndLoc()];
    transient.copyIndicesFrom(ind);
    
    transient.clearUVSets();

    std::vector<JMesh::UVSet>::const_iterator uvit = msh.uvSetBegin();
    for(;uvit!=msh.uvSetEnd();++uvit) {
        Float2 *uvd = transient.addUVSet(uvit->_name);
        const Float2 *uvs = (const Float2 *)&mem[uvit->_uv];
        memcpy(uvd, uvs, nt * 24);
    }

    transient.calculateVertexNormals();
    
    int npart = reduce(0, 0, &transient);
    
    return true;
}

void MeshListenerOps::computeMesh()
{
    const int n = numResources();
    if(n<1) return;
    
    simpleReform(m_lod, m_shoUV);
    
    drawableScene()->lock();
    for(int i=0;i<n;++i) {
        DrawableResource *rec = resource(i);
        processResourceNoLock(rec);
    }
    drawableScene()->unlock();
}

bool MeshListenerOps::hasMenu() const
{
    if(m_meshName == "unknown") return false;
    return true; 
}

std::string MeshListenerOps::meshCacheName() const
{ return m_meshName; }

}
