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
#include <h5_smp/HSurfaceSample.h>
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
	m_surfaceSamples = new SampleCacheTyp;
}

TerrainOps::~TerrainOps()
{
	delete m_surfaceSamples;
}

std::string TerrainOps::opsName() const
{ return "terrain"; }

bool TerrainOps::hasInstance() const
{ return false; }

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
        stat = ga.lsFirstTypedChild<HSurfaceSample>(smpName);
        ga.close();
		
		if(stat) {
			HSurfaceSample reader(smpName);
			stat = reader. template load<SampleCacheTyp>(m_surfaceSamples);
			reader.close();
		}
	}
	
	hio.end();
	
	if(stat) {
		
	} else {
		m_surfaceSamples->clear();
	}
	
	progress.setValue(1);
	return stat;
}

QString TerrainOps::getShortDescription() const
{
	QString r = VoxelOps::getShortDescription();
    return r + QString("\n n terrain sample %1").arg(m_surfaceSamples->numSamples()); 
}

bool TerrainOps::hasSurfaceSamples() const
{ return m_surfaceSamples->numSamples() > 0; }

const smp::SampleFilter<SurfaceSample> *TerrainOps::getSurfaceSamples() const
{ return m_surfaceSamples; }

} /// end of alo
