/*
 *  HorizonOps.cpp
 *  vachellia
 *
 */

#include "VoxelOps.h"
#include <interface/RenderableScene.h>
#include <math/raySphere.h>
#include <interface/IntersectResult.h>
#include <qt_base/AFileDlg.h>
#include <ssdf/SparseSignedDistanceField.h>
#include <h5/V1H5IO.h>
#include <h5_ssdf/HSsdf.h>
#include <QProgressDialog>
#include <QApplication>

namespace alo {
    
AFileDlgProfile VoxelOps::SReadProfile(AFileDlgProfile::FRead,
        "Choose File To Open",
        ":images/open_big.png",
        "Open Sparse Voxel Field File",
        "Open .hes",
        ".hes",
        "./",
        "");
   
VoxelOps::VoxelOps() : m_cachePath("unknown")
{
    m_field = new sdf::SsdField;
    m_rule = new sdf::SsdfLookupRule<sdf::SsdField>();
}

VoxelOps::~VoxelOps()
{
    delete m_rule;
    delete m_field;
}

void VoxelOps::addRenderableTo(RenderableScene *scene)
{
    setRenderableScene(scene);
    scene->enqueueCreateRenderable(this, opsId());
}
  
void VoxelOps::update()
{
    QAttrib * acp = findAttrib("cache_path");
    StringAttrib *fcp = static_cast<StringAttrib *>(acp);
    std::string scachePath;
    fcp->getValue(scachePath);
    if(m_cachePath != scachePath )
        loadCache(scachePath);
}

bool VoxelOps::intersectRay(const Ray& aray, IntersectResult& result)
{
    if(m_field->isEmpty()) 
        return RenderableObject::intersectRay(aray, result);

    float rayData[8];
    result.copyRayData(rayData);

    if(!rayAabbIntersect(rayData, aabb())) return false;

    float tt = rayData[6];
    const float &tLimit = rayData[7];
    Vector3F q = aray.travel(tt);

    for(int i=0;i<29;++i) {
        
        float d = m_rule->lookup((const float *)&q);
        if(d < tt * 1e-5f ) break;

        tt += d;
        if(tt > tLimit) return false;
        q = aray.travel(tt);
    }

    Vector3F tn = m_rule->lookupNormal((const float *)&q);
    //getNormalOnAabb((float *)&tn, (const float *)&q, aabb(), tt * 1e-5f);
    
    return result.updateRayDistance(tt, tn);
}

AFileDlgProfile *VoxelOps::readFileProfileR () const
{ return &SReadProfile; }

bool VoxelOps::loadCache(const std::string &fileName)
{
    QProgressDialog progress("Processing...", QString(), 0, 1, QApplication::activeWindow() );
    progress.setWindowModality(Qt::ApplicationModal);
    progress.show();

/// todo lock here
    ver1::H5IO hio;
    bool stat = hio.begin(fileName);
    if(!stat) {
        m_cachePath = "unknown";
        m_field->destroy();
        progress.setValue(1);
/// todo unlock here
        return stat;
    }

    stat = hio.ObjectExists("/asset");

    if(stat) {

        std::vector<std::string> fldNames;

        ver1::HBase ga("/asset");
        ga.lsTypedChild<HSsdf>(fldNames);
        ga.close();

        stat = fldNames.size() > 0;

        if(stat) {
            HSsdf reader(fldNames[0]);

            stat = reader. template load<sdf::SsdField>(*m_field);
    
            reader.close();
        }
    }

    hio.end();
/// todo unlock here
    if(stat) {
        m_cachePath = fileName;
        m_field->getBox(aabb());
        m_rule->attach(*m_field);
    }
    else {
        m_cachePath = "unknown";
        m_field->destroy();
        m_rule->dettach();
    }
    progress.setValue(1);
    return stat;

}

}