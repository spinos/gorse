/*
 *  VoxelOps.cpp
 *  vachellia
 *
 *  2019/4/29
 */

#include "VoxelOps.h"
#include <qt_graph/GlyphConnection.h>
#include <interface/RenderableScene.h>
#include <math/raySphere.h>
#include <interface/IntersectResult.h>
#include <qt_base/AFileDlg.h>
#include <ssdf/SsdField.h>
#include <ssdf/MultiLookupRule.h>
#include <grd/LocalGrid.h>
#include <grd/LocalGridLookupRule.h>
#include <h5/V1H5IO.h>
#include <h5_ssdf/HSsdf.h>
#include <h5_grd/HLocalGrid.h>
#include <QProgressDialog>
#include <QApplication>
#include <interface/GlobalFence.h>
#include <boost/thread/lock_guard.hpp>

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
    scene->createRenderable(this, opsTypeId());
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

    m_outOps.sendImpulse();
}

AFileDlgProfile *VoxelOps::readFileProfileR () const
{ return &SReadProfile; }

bool VoxelOps::loadCache(const std::string &fileName)
{
    interface::GlobalFence &fence = interface::GlobalFence::instance();
	boost::lock_guard<interface::GlobalFence> guard(fence);
        
    m_primitiveLookup->clear();
    m_gridRule->detach();

    QProgressDialog progress("Processing...", QString(), 0, 1, QApplication::activeWindow() );
    progress.setWindowModality(Qt::ApplicationModal);
    progress.show();

    ver1::H5IO hio;
    bool stat = hio.begin(fileName);
    if(!stat) {
        m_cachePath = "unknown";
        progress.setValue(1);
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

            m_grid->buildBvh();

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

void VoxelOps::connectTo(GlyphOps *another, const std::string &portName, GlyphConnection *line)
{
    m_outOps.append(line);
}

void VoxelOps::disconnectFrom(GlyphOps *another, const std::string &portName, GlyphConnection *line)
{
    m_outOps.remove(line);
}

bool VoxelOps::intersectRay(IntersectResult& result) const
{
    if(m_primitiveLookup->isEmpty() || m_gridRule->isEmpty() )
        return TransformOps::intersectRay(result);

    float rayData[8];
    result.copyRayData(rayData);

    rayToLocal(rayData);

    GridLookupRuleTyp::LookupResultTyp param;

    if(!m_gridRule->lookup(param, rayData)) return false;

    GridLookupRuleTyp::PrimitiveResultTyp &primitiveResult = param._primitive;

    rayData[6] = primitiveResult._t0;

    rayToWorld(rayData);
    normalToWorld((float *)&primitiveResult._nml);

    return result.updateRayDistance(rayData[6], primitiveResult._nml);
}

float VoxelOps::mapDistance(const float *q) const
{
    if(m_gridRule->isEmpty() )
        return TransformOps::mapDistance(q);

    float a[3];
    memcpy(a, q, 12);
    pointToLocal(a);

    GridLookupRuleTyp::PrimitiveResultTyp param;
    float d = m_gridRule->mapDistance(param, a);
    distanceToWorld(d);
    return d;
}

Vector3F VoxelOps::mapNormal(const float *q) const
{
    if(m_gridRule->isEmpty() )
        return TransformOps::mapNormal(q);
    
    float a[3];
    memcpy(a, q, 12);
    pointToLocal(a);

    GridLookupRuleTyp::PrimitiveResultTyp param;
    m_gridRule->mapNormal(param, a);
    
    Vector3F &tn = param._nml;
    normalToWorld((float *)&tn);
    return tn;
}

float VoxelOps::mapLocalDistance(const float *q) const
{
    if(m_gridRule->isEmpty() )
        return TransformOps::mapLocalDistance(q);

    float a[3];
    memcpy(a, q, 12);

    GridLookupRuleTyp::PrimitiveResultTyp param;
    return m_gridRule->mapDistance(param, a);
}

bool VoxelOps::hasInstance() const
{ return true; }

void VoxelOps::genSamples(sds::SpaceFillingVector<grd::PointSample> &samples) const
{
    if(m_gridRule->isEmpty() )
        return TransformOps::genSamples(samples);

    m_grid->genSamples<grd::PointSample>(samples);
}

QString VoxelOps::getShortDescription() const
{
    return QString("voxle file %1").arg(QString::fromStdString(m_cachePath)); 
}

} /// end of namespace alo
