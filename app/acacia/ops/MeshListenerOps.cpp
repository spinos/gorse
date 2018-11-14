#include "MeshListenerOps.h"
#include <QListWidget>
#include <qt_ogl/DrawableScene.h>
#include <qt_ogl/DrawableObject.h>
#include <geom/AdaptableMesh.h>
#include <sstream>
#include <boost/interprocess/mapped_region.hpp>
#include <ipc/SharedMemoryObject.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <jsn/JMesh.h>
#include <mesh/EdgeCollapse.h>
#include <mesh/HistoryMesh.h>
#include <mesh/HistoryReform.h>

using namespace boost::interprocess;
using namespace boost::property_tree;
    
namespace alo {
   
MeshListenerOps::MeshListenerOps() : m_upd(0), 
m_meshName("unknown"),
m_lod(.5f)
{ 
    m_mesh = new AdaptableMesh; 
    m_sourceMesh = new HistoryMesh;
    m_reformer = new HistoryReform;
}

MeshListenerOps::~MeshListenerOps()
{ 
    delete m_mesh; 
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
    
    computeMesh();

    DrawableScene *scene = drawableScene();
    DrawableObject *d = drawable();
    if(m_toRelocate || dataChanged || meshSelectionChanged) {
        scene->enqueueRemoveDrawable(d);
        setMeshDrawable(scene);
    } else {
        d->setPosnml((const float *)posnml.c_data(), posnml.capacityByteSize());
        d->setBarycentric((const float *)baryc.c_data(), baryc.capacityByteSize());
        d->setDrawArrayLength(m_mesh->numIndices());
        scene->enqueueEditDrawable(d);
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

void MeshListenerOps::setMeshDrawable(DrawableScene *scene)
{
    DrawableObject *cly = scene->createDrawable();
    cly->setPosnml((const float *)posnml.c_data(), posnml.capacityByteSize());
    cly->setBarycentric((const float *)baryc.c_data(), baryc.capacityByteSize());
    cly->setDrawArrayLength(m_mesh->numIndices());
    setDrawable(cly);
}

void MeshListenerOps::addDrawableTo(DrawableScene *scene)
{ 
    computeMesh();
    setDrawableScene(scene);
    setMeshDrawable(scene);
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
/// out of date
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
    if(m_meshName == "unknown") {
        m_mesh->initAdaptableMesh();
        m_mesh->addVertex(Vector3F(0.f, 0.f, 0.f));
        m_mesh->addVertex(Vector3F(8.f, 0.f, 0.f));
        m_mesh->addVertex(Vector3F(8.f, 8.f, 0.f));
        m_mesh->addVertex(Vector3F(0.f, 8.f, 0.f));
        m_mesh->addTriangle(0, 1, 2);
        m_mesh->addTriangle(0, 2, 3);
        m_mesh->calculateVertexNormals();
    } else {
        m_reformer->reform(m_mesh, m_lod, m_sourceMesh);
    }
    
    const int oldL = posnml.capacity();
    m_mesh->createPositionNormalArray(posnml);
    m_toRelocate = oldL < posnml.capacity();
    if(m_toRelocate) m_mesh->createBarycentricCoordinates(baryc);
}

bool MeshListenerOps::hasMenu() const
{ return true; }

void MeshListenerOps::getMenuItems(std::vector<std::pair<std::string, int > > &ks) const 
{
    ks.push_back(std::make_pair("Save", 1));
}

void MeshListenerOps::recvAction(int x) 
{
    std::cout<<" MeshListenerOps::recvAction ";
    if(x == 1) std::cout << " todo save ";
}

}

