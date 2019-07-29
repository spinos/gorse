/*
 *  TerrainOps.cpp
 *  vachellia
 *
 *  2019/7/29
 */

#include "TerrainOps.h"
#include <smp/SurfaceGeodesicSample.h>
#include <smp/SampleFilter.h>
#include <qt_base/AFileDlg.h>
#include <h5/V1H5IO.h>
#include <h5_smp/HSurfaceGeodesicSample.h>
#include <QProgressDialog>
#include <QApplication>
#include <QMessageBox>
#include <QDebug>

namespace alo {
	
AFileDlgProfile TerrainOps::SReadTrunkProfile(AFileDlgProfile::FRead,
        "Choose File To Open",
        ":images/open_big.png",
        "Open Sparse Voxel Field File + Terrain Samples",
        "Open .hes",
        ".hes",
        "./",
        "");
   
TerrainOps::TerrainOps()
{
	m_geodesicSamples = new SampleCacheTyp;
}

TerrainOps::~TerrainOps()
{
	delete m_geodesicSamples;
}

std::string TerrainOps::opsName() const
{ return "terrain"; }

AFileDlgProfile *TerrainOps::readFileProfileR () const
{ return &SReadTrunkProfile; }

bool TerrainOps::loadSsdfCache(const std::string &fileName)
{
	bool stat = VoxelOps::loadSsdfCache(fileName);
	if(!stat) return stat;
	
	QProgressDialog progress("Processing...", QString(), 0, 1, QApplication::activeWindow() );
    progress.setWindowModality(Qt::ApplicationModal);
    progress.show();
	
	ver1::H5IO hio;
    stat = hio.begin(fileName);
	if(!stat) return stat;
	
	stat = hio.ObjectExists("/asset");
    if(stat) {
		int nfld = 0;
		std::string smpName;

        ver1::HBase ga("/asset");
        stat = ga.lsFirstTypedChild<HSurfaceGeodesicSample>(smpName);
        ga.close();
		
		if(stat) {
			HSurfaceGeodesicSample reader(smpName);
			stat = reader. template load<SampleCacheTyp>(m_geodesicSamples);
			reader.close();
		}
	}
	
	hio.end();
	
	if(stat) {
		
	} else {
		m_geodesicSamples->clear();
	}
	
	progress.setValue(1);
	return stat;
}

QString TerrainOps::getShortDescription() const
{
	QString r = VoxelOps::getShortDescription();
    return r + QString("\n n terrain sample %1").arg(m_geodesicSamples->numSamples()); 
}

bool TerrainOps::hasGeodesicSamples() const
{ return m_geodesicSamples->numSamples() > 0; }

const smp::SampleFilter<SurfaceGeodesicSample> *TerrainOps::getGeodesicSamples() const
{ return m_geodesicSamples; }

} /// end of alo
