/*
 *  VoxelOps.cpp
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

std::string VoxelOps::opsName() const
{ return "voxel"; }

void VoxelOps::addRenderableTo(RenderableScene *scene)
{
    setRenderableScene(scene);
    scene->enqueueCreateRenderable(this, opsId());
}
  
void VoxelOps::update()
{
    TransformOps::update();
    QAttrib * acp = findAttrib("cache_path");
    StringAttrib *fcp = static_cast<StringAttrib *>(acp);
    std::string scachePath;
    fcp->getValue(scachePath);
    if(m_cachePath != scachePath && scachePath.size() > 3)
        loadCache(scachePath);
    m_outOps.update();
}

bool VoxelOps::intersectRay(const Ray& aray, IntersectResult& result)
{
    if(m_field->isEmpty()) 
        return TransformOps::intersectRay(aray, result);

    float rayData[8];
    result.copyRayData(rayData);

    rayToLocal(rayData);

    if(!rayAabbIntersect(rayData, aabb())) return false;

    float &tt = rayData[6];
    const float &tLimit = rayData[7];
    float q[3];

    for(int i=0;i<256;++i) {
        
        rayTravel(q, rayData);

        float d = m_rule->lookup(q);

        if(d < 1e-3f) break;

        if(d < tt * 1e-5f) break;

        tt += d * .8f;
     
        if(tt > tLimit) return false;
    }

    Vector3F tn = mapNormal(q);

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

void VoxelOps::connectTo(GlyphOps *another, const std::string &portName)
{
    RenderableOps *r = static_cast<RenderableOps *>(another);
    std::cout << "\n VoxelOps " << this << " connectTo renderable " << r;
    m_outOps.append(r);
}

void VoxelOps::disconnectFrom(GlyphOps *another, const std::string &portName)
{
    RenderableOps *r = static_cast<RenderableOps *>(another);
    m_outOps.remove(r);
    std::cout << "\n VoxelOps " << this << " disconnectFrom renderable " << r;
}

float VoxelOps::mapDistance(const float *q) const
{
    if(m_field->isEmpty()) 
        return TransformOps::mapDistance(q);

    float a[3];
    memcpy(a, q, 12);
    pointToLocal(a);

    return m_rule->lookup(a);
}

Vector3F VoxelOps::mapNormal(const float *q) const
{
    if(m_field->isEmpty()) 
        return TransformOps::mapNormal(q);
    
    Vector3F tn = m_rule->lookupNormal(q);
    normalToWorld((float *)&tn);
    return tn;
}

}
