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
#include <ssdf/MultiLookupRule.h>
#include <grd/LocalGrid.h>
#include <grd/LocalGridLookupRule.h>
#include <h5/V1H5IO.h>
#include <h5_ssdf/HSsdf.h>
#include <h5_ssdf/HLocalGrid.h>
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
    m_primitiveLookup = new PrimitiveLookupRule;
    m_gridRule = new GridLookupRuleTyp;
    m_gridRule->setPrimitiveRule(m_primitiveLookup);
    m_grid = new GridTyp;
}

VoxelOps::~VoxelOps()
{
    delete m_gridRule;
    delete m_grid;
    delete m_primitiveLookup;
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
    int mns;
    getIntAttribValue(mns, "amaxnstep");
    m_gridRule->setMaxNumStep(mns);

    QAttrib * acp = findAttrib("cache_path");
    StringAttrib *fcp = static_cast<StringAttrib *>(acp);
    std::string scachePath;
    fcp->getValue(scachePath);
    if(m_cachePath != scachePath && scachePath.size() > 3)
        loadCache(scachePath);

    m_primitiveLookup->setAllRelativeBoundaryOffset(boundary);
    m_outOps.update();
}

AFileDlgProfile *VoxelOps::readFileProfileR () const
{ return &SReadProfile; }

bool VoxelOps::loadCache(const std::string &fileName)
{
    m_primitiveLookup->clear();
    m_gridRule->detach();

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
    int nfld = 0;
    if(stat) {

        std::vector<std::string> fldNames;
        std::string grdName;

        ver1::HBase ga("/asset");
        ga.lsTypedChild<HSsdf>(fldNames);
        bool hasGrd = ga.lsFirstTypedChild<HLocalGrid>(grdName);
        ga.close();

        stat = hasGrd && (fldNames.size() > 0);

        if(stat) {

            HLocalGrid greader(grdName);
            greader.load<GridTyp>(*m_grid);
            greader.close();

            nfld = fldNames.size();

            m_primitiveLookup->create(nfld);

            for(int i=0;i<nfld;++i) {

                HSsdf reader(fldNames[i]);

                stat = reader. template load<sdf::SsdField>(m_primitiveLookup->field(i));
    
                reader.close();

                m_primitiveLookup->attach(i);
            } 
        }
    }

    hio.end();
/// todo unlock here
    if(stat) {
        m_cachePath = fileName;
        m_gridRule->attach(m_grid);
        memcpy(aabb(), &m_grid->bbox(), 24);
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

bool VoxelOps::intersectRay(const Ray& aray, IntersectResult& result)
{
    if(m_primitiveLookup->isEmpty() || m_gridRule->isEmpty() )
        return TransformOps::intersectRay(aray, result);

    float rayData[8];
    result.copyRayData(rayData);

    rayToLocal(rayData);

    GridLookupResultTyp param;

    if(!m_gridRule->lookup(param, rayData)) return false;

    rayData[6] = param._t0;

    rayToWorld(rayData);
    normalToWorld((float *)&param._nml);

    return result.updateRayDistance(rayData[6], param._nml);
}

float VoxelOps::mapDistance(const float *q) const
{
    //if(m_numPairs < 1 || m_gridRule->isEmpty() )
        return TransformOps::mapDistance(q);

   /* float a[3];
    memcpy(a, q, 12);
    pointToLocal(a);

    GridLookupResultTyp param;
    return mapLocalDistanceTo(a, param);*/
}

Vector3F VoxelOps::mapNormal(const float *q) const
{
    //if(m_numPairs < 1 || m_gridRule->isEmpty() )
        return TransformOps::mapNormal(q);
    
   /* float a[3];
    memcpy(a, q, 12);
    pointToLocal(a);

    GridLookupResultTyp param;
    Vector3F tn = mapLocalNormalAt(a, param);
    
    normalToWorld((float *)&tn);
    return tn;*/
}

} /// end of namespace alo
