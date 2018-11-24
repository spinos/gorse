#include "PVSTest.h"
#include <qt_ogl/DrawableScene.h>
#include <qt_ogl/DrawableObject.h>
#include <geom/AdaptableMesh.h>
#include <qt_base/AFileDlg.h>
#include <h5/V1H5IO.h>
#include <h5/V1HBase.h>
#include <qt_ogl/CameraEvent.h>
#include <math/AFrustum.h>

namespace alo {
   
PVSTest::PVSTest() :
m_drawLength(0)
{
}

PVSTest::~PVSTest()
{
}
    
void PVSTest::update()
{
    QAttrib * acp = findAttrib("cache_path");
    StringAttrib *fcp = static_cast<StringAttrib *>(acp);
    std::string scachePath;
    fcp->getValue(scachePath);
    
    //if(cachePathChanged(scachePath) )
    //    loadCache(scachePath);
    
    computeMesh();

    DrawableScene *scene = drawableScene();
    DrawableObject *d = drawable();
    if(m_toRelocate) {
        scene->enqueueRemoveDrawable(d);
        setMeshDrawable(scene);
    } else {
        d->setPosnml((const float *)posnml.c_data(), posnml.capacityByteSize());
        d->setBarycentric((const float *)baryc.c_data(), baryc.capacityByteSize());
        d->setDrawArrayLength(m_drawLength);
        scene->enqueueEditDrawable(d);
    }
}

void PVSTest::setMeshDrawable(DrawableScene *scene)
{
    DrawableObject *cly = scene->createDrawable();
    cly->setPosnml((const float *)posnml.c_data(), posnml.capacityByteSize());
    cly->setBarycentric((const float *)baryc.c_data(), baryc.capacityByteSize());
    cly->setDrawArrayLength(m_drawLength);
    setDrawable(cly);
}

void PVSTest::addDrawableTo(DrawableScene *scene)
{ 
    computeMesh();
    setDrawableScene(scene);
    setMeshDrawable(scene);
}

void PVSTest::computeMesh()
{
    AdaptableMesh transient;
    transient.createMinimal();
    m_drawLength = transient.numIndices();
    
    const int oldL = posnml.capacity();
    transient.createPositionNormalArray(posnml);
    m_toRelocate = oldL < posnml.capacity();
    if(m_toRelocate) transient.createBarycentricCoordinates(baryc);
}

void PVSTest::recvCameraChanged(const CameraEvent &x)
{
    std::cout << "\n camera frustum " << *(x.frustum());
}

}
