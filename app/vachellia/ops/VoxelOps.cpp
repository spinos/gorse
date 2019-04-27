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
   
VoxelOps::VoxelOps() : m_cachePath("unknown"),
m_maxNStep(128),
m_pairs(nullptr),
m_numPairs(0)
{
}

VoxelOps::~VoxelOps()
{
    clearAllPairs();
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

    float boundary;
    getFloatAttribValue(boundary, "bthickness");
    getIntAttribValue(m_maxNStep, "amaxnstep");

    QAttrib * acp = findAttrib("cache_path");
    StringAttrib *fcp = static_cast<StringAttrib *>(acp);
    std::string scachePath;
    fcp->getValue(scachePath);
    if(m_cachePath != scachePath && scachePath.size() > 3)
        loadCache(scachePath);

    setAllRelativeBoundaryOffset(boundary);
    m_outOps.update();
}

bool VoxelOps::intersectRay(const Ray& aray, IntersectResult& result)
{
    if(m_numPairs < 1)
        return TransformOps::intersectRay(aray, result);

    float rayData[8];
    result.copyRayData(rayData);

    rayToLocal(rayData);

    if(!rayAabbIntersect(rayData, aabb())) return false;

    float &tt = rayData[6];
    const float &tLimit = rayData[7];
    float q[3];
    int objId;

    for(int i=0;i<m_maxNStep;++i) {
        
        rayTravel(q, rayData);

        float d = mapLocalDistanceTo(q, objId);

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
    clearAllPairs();

    QProgressDialog progress("Processing...", QString(), 0, 1, QApplication::activeWindow() );
    progress.setWindowModality(Qt::ApplicationModal);
    progress.show();

/// todo lock here
    ver1::H5IO hio;
    bool stat = hio.begin(fileName);
    if(!stat) {
        m_cachePath = "unknown";
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
            int n = fldNames.size();

            m_pairs = new FieldLookupRulePair[n];

            for(int i=0;i<n;++i) {

                FieldLookupRulePair &p = m_pairs[i];
                p._field = new sdf::SsdField;
                p._rule = new LookupRuleTyp;

                HSsdf reader(fldNames[i]);

                stat = reader. template load<sdf::SsdField>(*p._field);
    
                reader.close();

                p._rule->attach(*p._field);
            }

            m_numPairs = n;
        }
    }

    hio.end();
/// todo unlock here
    if(stat) {
        m_cachePath = fileName;
        updateAllPairsAabb();
    }
    else {
        m_cachePath = "unknown";
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
    if(m_numPairs < 1)
        return TransformOps::mapDistance(q);

    float a[3];
    memcpy(a, q, 12);
    pointToLocal(a);

    int i;
    return mapLocalDistanceTo(a, i);
}

Vector3F VoxelOps::mapNormal(const float *q) const
{
    if(m_numPairs < 1)
        return TransformOps::mapNormal(q);
    
    int i;
    mapLocalDistanceTo(q, i);

    const LookupRuleTyp *r = m_pairs[i]._rule;

    Vector3F tn = r->lookupNormal(q);
    normalToWorld((float *)&tn);
    return tn;
}

void VoxelOps::clearAllPairs()
{
    if(m_numPairs<1) return;
    const int n = m_numPairs;
    m_numPairs = 0;
    for(int i=0;i<n;++i) {
        FieldLookupRulePair &p = m_pairs[i];
        delete p._field;
        delete p._rule;
    }
    delete[] m_pairs;
}

void VoxelOps::updateAllPairsAabb()
{
    setAabbNull();
    for(int i=0;i<m_numPairs;++i) {
        const sdf::SsdField *r = m_pairs[i]._field;
        r->expandAabb(aabb());
    }
}

void VoxelOps::setAllRelativeBoundaryOffset(float x)
{
    if(m_numPairs<1) return;
    for(int i=0;i<m_numPairs;++i) {
        LookupRuleTyp *r = m_pairs[i]._rule;
        r->setRelativeBoundaryOffset(x);
    }
}

float VoxelOps::mapLocalDistanceTo(const float *q, int &objI) const
{
    float md = 1e10f;
    for(int i=0;i<m_numPairs;++i) {
        float d = m_pairs[i]._rule->lookup(q);
        if(md > d) {
            md = d;
            objI = i;
        }
    }
    return md;
}

}
