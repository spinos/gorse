
#include "mainwindow.h"
#include <QLabel>
#include <QDebug>
#include <qt_base/AFileDlg.h>

using namespace alo;

MainWindow::MainWindow()
{
    AFileDlgProfile prof(AFileDlgProfile::FWrite,
    	"Choose File To Save",
        ":images/unknown.png",
    	"Save mesh to file\nNv\nNt",
        "Save .h5",
    	".h5",
    	"./",
    	"untitled");
    
    AFileDlg d(prof, this);
    int res = d.exec();
    if(res == QDialog::Rejected)
        qDebug()<<"abort save";
    
    QLabel *la = new QLabel(tr(prof.getFilePath().c_str()));
    setCentralWidget(la);
}
