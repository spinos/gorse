/*
 *  MeshListener.cpp
 *  acacia
 *
 */

#include "MeshListener.h"
#include <QListWidget>
#include <qt_ogl/DrawableScene.h>
#include <boost/interprocess/mapped_region.hpp>
#include <ipc/SharedMemoryObject.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <jsn/JMesh.h>

using namespace boost::interprocess;
using namespace boost::property_tree;
    
namespace alo {

MeshListener::MeshListener() : m_upd(0), 
m_meshName("unknown")
{}

MeshListener::~MeshListener()
{}
    
bool MeshListener::checkMeshChange(bool &dataChanged)
{
    std::string meshName;
    getListAttribValue(meshName, "ipc_mesh_list");
    
    const bool meshSelectionChanged = (m_meshName != meshName);
    m_meshName = meshName;
    
    dataChanged = checkBroadcastTime();
    
    return dataChanged || meshSelectionChanged;
}

void MeshListener::postUI()
{
    QAttrib * ameshes = findAttrib("ipc_mesh_list");
    ListAttrib *fmeshes = static_cast<ListAttrib *>(ameshes);
    QListWidget *lmeshes = fmeshes->widget();

    listAvailableMeshes(lmeshes);
}

void MeshListener::listAvailableMeshes(QListWidget *wig)
{
    m_meshMap.clear();
    
    wig->clear();

    SharedMemoryObject shm;
    mapped_region region = shm.readRegion("MySharedMemory");
    if(region.get_size() < 1) {
        m_meshName = "unknown";
        return;
    }

    const char *mem = static_cast<const char*>(region.get_address());
    std::stringstream sst(mem);
    std::cout<<" "<<sst.str();
    
    bool found = false;
    ptree pt;
    json_parser::read_json(sst, pt);
    BOOST_FOREACH(ptree::value_type const&v, pt.get_child("meshes")) {

        std::string meshName = v.second.get<std::string>("name");
        m_meshMap[meshName] = JMesh();
        m_meshMap[meshName].readPtree(v.second);
        
        wig->addItem(QString(meshName.c_str()) );

        if(meshName == m_meshName) found = true;
    }

    if(!found) m_meshName = "unknown";
    
}

void MeshListener::addDrawableTo(DrawableScene *scene)
{ 
    setDrawableScene(scene);
}

bool MeshListener::checkBroadcastTime()
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

bool MeshListener::loadMeshMaster()
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

bool MeshListener::loadMesh(AdaptableMesh *transient, bool dataChanged)
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
    
    transient->createTriangleMesh(nv, nt);
    
    const char *mem = static_cast<const char*>(region.get_address());
    const Vector3F *pos = (const Vector3F *)&mem[msh.getPosLoc()];
    transient->copyPositionsFrom(pos);
    const unsigned *ind = (const unsigned *)&mem[msh.getIndLoc()];
    transient->copyIndicesFrom(ind);
    
    transient->clearUVSets();

    std::vector<JMesh::UVSet>::const_iterator uvit = msh.uvSetBegin();
    for(;uvit!=msh.uvSetEnd();++uvit) {
        Float2 *uvd = transient->addUVSet(uvit->_name);
        const Float2 *uvs = (const Float2 *)&mem[uvit->_uv];
        memcpy(uvd, uvs, nt * 24);
    }

    transient->calculateVertexNormals();
    return true;
}

bool MeshListener::hasMenu() const
{
    if(m_meshName == "unknown") return false;
    return true; 
}

std::string MeshListener::meshCacheName() const
{ return m_meshName; }

}
