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
#include <QMessageBox>
#include <QDebug>

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
m_assetName("unknown")
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
/// reload?
    if(m_cachePath != scachePath && scachePath.size() > 3)
        loadSsdfCache(scachePath);

    m_primitiveLookup->setAllRelativeBoundaryOffset(boundary);

    m_outOps.sendImpulse();
}

AFileDlgProfile *VoxelOps::readFileProfileR () const
{ return &SReadProfile; }

bool VoxelOps::loadSsdfCache(const std::string &fileName)
{    
    qDebug()<< " load cache "<<QString::fromStdString(fileName);
    m_primitiveLookup->clear();
    m_gridRule->detach();

    ver1::H5IO hio;
    bool stat = hio.begin(fileName);
    if(!stat) {
        m_cachePath = "unknown";
		m_assetName = "unknown";
        QMessageBox msgBox;
        msgBox.setText(QString("Cannot open file %1").arg(QString::fromStdString(fileName)));
        msgBox.setInformativeText("The file does not exist or it is not a HDF5 file.");
        msgBox.exec();
        return stat;
    }

    QProgressDialog progress("Processing...", QString(), 0, 1, QApplication::activeWindow() );
    progress.setWindowModality(Qt::ApplicationModal);
    progress.show();

    stat = hio.ObjectExists("/asset");
    if(stat) {
		int nfld = 0;
        std::vector<std::string> fldNames;
        std::string grdName;

        ver1::HBase ga("/asset");
        ga.lsTypedChild<HSsdf>(fldNames);
        bool hasGrd = ga.lsFirstTypedChild<HLocalGrid>(grdName);
		if(ga.hasNamedAttr(".assetname")) {
			ga.readVLStringAttr(".assetname", m_assetName);
		} else {
			m_assetName = "unknown";
		}
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
        QMessageBox msgBox;
        msgBox.setText(QString("Cannot load voxel from file %1").arg(QString::fromStdString(fileName)));
        msgBox.setInformativeText("The file contains no voxel asset.");
        msgBox.exec();
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

bool VoxelOps::hasGeodesicSamples() const
{ return false; }

bool VoxelOps::hasSurfaceSamples() const
{ return false; }

void VoxelOps::genSamples(sds::SpaceFillingVector<grd::CellSample> &samples) const
{
    if(m_gridRule->isEmpty() )
        return TransformOps::genSamples(samples);

    m_grid->genSamples<grd::CellSample>(samples);
}

QString VoxelOps::getShortDescription() const
{
    return QString(" file %1 \n asset %2 ").arg(QString::fromStdString(m_cachePath),
												QString::fromStdString(m_assetName)); 
}

const std::string &VoxelOps::assetName() const
{ return m_assetName; }

} /// end of namespace alo
