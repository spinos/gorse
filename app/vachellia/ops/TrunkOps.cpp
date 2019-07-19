/*
 *  TrunkOps.cpp
 *  vachellia
 *
 *  2019/7/15
 */

#include "TrunkOps.h"
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
	
AFileDlgProfile TrunkOps::SReadTrunkProfile(AFileDlgProfile::FRead,
        "Choose File To Open",
        ":images/open_big.png",
        "Open Sparse Voxel Field File + Trunk Samples",
        "Open .hes",
        ".hes",
        "./",
        "");
   
TrunkOps::TrunkOps()
{
	m_geodesicSamples = new SampleCacheTyp;
}

TrunkOps::~TrunkOps()
{
	delete m_geodesicSamples;
}

std::string TrunkOps::opsName() const
{ return "tree_trunk"; }

AFileDlgProfile *TrunkOps::readFileProfileR () const
{ return &SReadTrunkProfile; }

bool TrunkOps::loadSsdfCache(const std::string &fileName)
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

QString TrunkOps::getShortDescription() const
{
	QString r = VoxelOps::getShortDescription();
    return r + QString("\n n geodesic sample %1").arg(m_geodesicSamples->numSamples()); 
}

bool TrunkOps::hasGeodesicSamples() const
{ return m_geodesicSamples->numSamples() > 0; }

} /// end of namespace alo
