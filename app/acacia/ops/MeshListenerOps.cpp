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

namespace alo {
   
MeshListenerOps::MeshListenerOps()
{ 
    m_mesh = new AdaptableMesh; 
}

MeshListenerOps::~MeshListenerOps()
{ 
    delete m_mesh; 
}
    
void MeshListenerOps::update()
{
    QAttrib * alod = findAttrib("lod");
    FloatAttrib *flod = static_cast<FloatAttrib *>(alod);
    //fa->getValue(m_profile._a);
    
    computeMesh();

    DrawableScene *scene = drawableScene();
    DrawableObject *d = drawable();
    if(m_toRelocate) {
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
    using namespace boost::interprocess;
    using namespace boost::property_tree;

    wig->clear();

    SharedMemoryObject shm;
    mapped_region region = shm.readRegion("MySharedMemory");
    if(region.get_size() < 1) return;

    const char *mem = static_cast<const char*>(region.get_address());
    std::stringstream sst(mem);
    ptree pt;
    json_parser::read_json(sst, pt);
    BOOST_FOREACH(ptree::value_type const&v, pt.get_child("meshes")) {

        JMesh msh;
        msh.readPtree(v.second);
        wig->addItem(QString(msh.name().c_str()) );
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

void MeshListenerOps::computeMesh()
{
    m_mesh->initAdaptableMesh();

    m_mesh->addVertex(Vector3F(0.f, 0.f, 0.f));
    m_mesh->addVertex(Vector3F(8.f, 0.f, 0.f));
    m_mesh->addVertex(Vector3F(8.f, 8.f, 0.f));
    m_mesh->addVertex(Vector3F(0.f, 8.f, 0.f));
    m_mesh->addTriangle(0, 1, 2);
    m_mesh->addTriangle(0, 2, 3);

    m_mesh->calculateVertexNormals();
    const int oldL = posnml.capacity();
    m_mesh->createPositionNormalArray(posnml);
    m_toRelocate = oldL < posnml.capacity();
    if(m_toRelocate) m_mesh->createBarycentricCoordinates(baryc);
}

}

