/*
 *  VoxelOps.cpp
 *  vachellia
 *
 *  2019/4/29
 */

#include "VoxelOps.h"
#include <interface/RenderableScene.h>
#include <math/raySphere.h>
#include <interface/IntersectResult.h>
#include <qt_base/AFileDlg.h>
#include <ssdf/SparseSignedDistanceField.h>
#include <grd/IndexGrid.h>
#include <grd/IndexGridLookupRule.h>
#include <h5/V1H5IO.h>
#include <h5_ssdf/HSsdf.h>
#include <h5_ssdf/HIndexGrid.h>
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
    m_grid = new grd::IndexGrid;
    m_gridRule = new GridLookupRuleTyp;
}

VoxelOps::~VoxelOps()
{
    clearAllPairs();
    delete m_gridRule;
    delete m_grid;
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
    if(m_numPairs < 1 || m_gridRule->isEmpty() )
        return TransformOps::intersectRay(aray, result);

    float rayData[8];
    result.copyRayData(rayData);

    rayToLocal(rayData);

    if(!rayAabbIntersect(rayData, aabb())) return false;

    float &tt = rayData[6];
    const float &tLimit = rayData[7];
    float q[3];
    GridLookupResultTyp param;

    for(int i=0;i<m_maxNStep;++i) {
        
        rayTravel(q, rayData);

        float d = mapLocalDistanceTo(q, param);

        if(d < 1e-3f) break;

        if(d < tt * 1e-5f) break;

        tt += d;
     
        if(tt > tLimit) return false;
    }

    Vector3F tn = mapLocalNormalAt(q, param);
    normalToWorld((float *)&tn);
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
        m_gridRule->detach();
        progress.setValue(1);
/// todo unlock here
        return stat;
    }

    stat = hio.ObjectExists("/asset");
    int nfld = 0;
    if(stat) {

        std::vector<std::string> fldNames;
        std::string grdName;

        ver1::HBase ga("/asset");
        ga.lsTypedChild<HSsdf>(fldNames);
        bool hasGrd = ga.lsFirstTypedChild<HIndexGrid>(grdName);
        ga.close();

        stat = hasGrd && (fldNames.size() > 0);

        if(stat) {

            HIndexGrid greader(grdName);
            greader.load(*m_grid);
            greader.close();

            nfld = fldNames.size();

            m_pairs = new FieldLookupRulePair[nfld];

            for(int i=0;i<nfld;++i) {

                FieldLookupRulePair &p = m_pairs[i];
                p._field = new sdf::SsdField;
                p._rule = new LookupRuleTyp;

                HSsdf reader(fldNames[i]);

                stat = reader. template load<sdf::SsdField>(*p._field);
    
                reader.close();

                p._rule->attach(*p._field);
            } 
        }
    }

    hio.end();
/// todo unlock here
    if(stat) {
        m_cachePath = fileName;
        m_numPairs = nfld;
        m_gridRule->attach(*m_grid);
        memcpy(aabb(), m_grid->aabb(), 24);
    }
    else {
        m_cachePath = "unknown";
        m_gridRule->detach();
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
    if(m_numPairs < 1 || m_gridRule->isEmpty() )
        return TransformOps::mapDistance(q);

    float a[3];
    memcpy(a, q, 12);
    pointToLocal(a);

    GridLookupResultTyp param;
    return mapLocalDistanceTo(a, param);
}

Vector3F VoxelOps::mapNormal(const float *q) const
{
    if(m_numPairs < 1 || m_gridRule->isEmpty() )
        return TransformOps::mapNormal(q);
    
    float a[3];
    memcpy(a, q, 12);
    pointToLocal(a);

    GridLookupResultTyp param;
    Vector3F tn = mapLocalNormalAt(a, param);
    
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

void VoxelOps::setAllRelativeBoundaryOffset(float x)
{
    if(m_numPairs<1) return;

    for(int i=0;i<m_numPairs;++i) {
        LookupRuleTyp *r = m_pairs[i]._rule;
        r->setRelativeBoundaryOffset(x);
    }
}

float VoxelOps::mapLocalDistanceTo(const float *q, GridLookupResultTyp &result) const
{
    m_gridRule->lookup(result, q);
    if(result.isEmptySpace() 
        && result._distance > m_grid->cellSize() ) {
        return result._distance * .7f;
    }
        
    findObjectClosestTo(q, result);
    
    result._distance *= .732f;
    m_pairs[result._instanceId]._rule->limitStepSize(result._distance);

    return result._distance;
}

Vector3F VoxelOps::mapLocalNormalAt(const float *q, GridLookupResultTyp &result) const
{
    if(result._instanceId < 0)
        findObjectClosestTo(q, result);

    const LookupRuleTyp *r = m_pairs[result._instanceId]._rule;

    return r->lookupNormal(q);
}

void VoxelOps::findObjectClosestTo(const float *q, GridLookupResultTyp &result) const 
{
    if(result.hasInstanceRange()) return findObjectInRangeClosestTo(q, result);
    
    float &md = result._distance;
    md = 1e10f;
    for(int i=0;i<m_numPairs;++i) {
        float d = m_pairs[i]._rule->lookup(q);
        if(md > d) {
            md = d;
            result._instanceId = i;
        }
    }
    
}

void VoxelOps::findObjectInRangeClosestTo(const float *q, GridLookupResultTyp &result) const 
{
    const int *ind = m_grid->c_indices();
    float &md = result._distance;
    md = 1e10f;
    for(int i=result._instanceRange.x;i<result._instanceRange.y;++i) {
        const int &objI = ind[i];

        float d = m_pairs[objI]._rule->lookup(q);
        if(md > d) {
            md = d;
            result._instanceId = objI;
        }
    }
}

} /// end of namespace alo
