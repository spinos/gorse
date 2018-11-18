/*
 *  AFileDlg.cpp
 *  aloe
 *
 *
 */

#include "AFileDlg.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QFileDialog>
#include <QListWidget>
#include <QPixmap>
#include <QDir>
#include "ListSelector.h"

namespace alo {
    
AFileDlg::Profile::Profile()
{}

AFileDlg::Profile::Profile(FileMode mode,
    const std::string &title,
    const std::string &iconName,
    const std::string &notice,
    const std::string &applyName,
    const std::string &ext,
    const std::string &folder,
    const std::string &name)
{
    _mode = mode;
    _title = title;
    _iconName = iconName;
    _notice = notice;
    _applyName = applyName;
    _ext = ext;
    _folder = folder;
    _name = name;
}

void AFileDlg::Profile::setName(const std::string &x)
{
    _name = x;
    int l = _name.rfind(_ext);
    if(l>1) _name.erase(l);
}

void AFileDlg::Profile::setFolder(const std::string &x)
{
/// remove repeated slashes
    struct both_slashes {
        bool operator()(char a, char b) const {
            return a == '/' && b == '/';
        }
    };

    std::string spath(x);
    spath.erase(std::unique(spath.begin(), spath.end(), both_slashes()), spath.end());

    if(spath.rfind('/') == spath.size() - 1) {
        _folder = spath;
        return;
    }
    _folder = spath + '/';
}

std::string AFileDlg::Profile::getFilePath() const
{ return _folder + _name + _ext; }


AFileDlg::AFileDlg(Profile &prof, QWidget *parent) : QDialog(parent)
{
    m_profile = &prof;
    
	setWindowTitle(tr(prof._title.c_str()) );
	
    QLabel *icn = new QLabel(this);
    icn->setPixmap(QPixmap(tr(prof._iconName.c_str()) ) );
    icn->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    m_notice = new QLabel(tr(prof._notice.c_str()), this);
	m_notice->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	m_notice->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    QHBoxLayout *noticeLayout = new QHBoxLayout;
    noticeLayout->addWidget(icn);
    noticeLayout->addWidget(m_notice);
    
    m_folderEdt = new QLineEdit(tr(prof._folder.c_str()), this);
    QPushButton *browseBtn = new QPushButton(tr("Browse"));
    
    QHBoxLayout *browseLayout = new QHBoxLayout;
    browseLayout->addWidget(m_folderEdt);
	browseLayout->addWidget(browseBtn);
    
    m_nameEdt = new QLineEdit(tr(prof._name.c_str()), this);
    m_applyBtn = new QPushButton(tr(prof._applyName.c_str()), this);
	m_cancelBtn = new QPushButton(tr("Cancel"), this);
	
    QHBoxLayout *nameLayout = new QHBoxLayout;
    nameLayout->addWidget(m_nameEdt);
    nameLayout->addWidget(m_applyBtn);
    nameLayout->addWidget(m_cancelBtn);

    m_fileList = new ListSelector(this);

	QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(noticeLayout);
    mainLayout->addLayout(browseLayout);
	mainLayout->addLayout(nameLayout);
    mainLayout->addWidget(m_fileList);
	setLayout(mainLayout);
	resize(400, 300);
    
    connect(browseBtn, SIGNAL(clicked()), 
			this, SLOT(getFolderPath()));
			
    connect(m_applyBtn, SIGNAL(clicked()), 
			this, SLOT(accept()));
            
	connect(m_cancelBtn, SIGNAL(clicked()), 
			this, SLOT(reject()));
            
    connect(m_fileList, SIGNAL(selectionChanged(QPair<std::string, std::string>)),
            this, SLOT(recvListItemSelection(QPair<std::string, std::string>)) );
    
    if(prof._mode == FRead)
        m_nameEdt->setReadOnly(true);
    else
        connect(m_nameEdt, SIGNAL(textEdited(QString)),
            this, SLOT(recvNameChange(QString)) );
            
    connect(m_folderEdt, SIGNAL(textEdited(QString)),
            this, SLOT(recvFolderChange(QString)) );
            
    lsFilesInFolder();
    
}

void AFileDlg::getFolderPath()
{
	QString fn = QFileDialog::getExistingDirectory(this,
			tr("Select A Folder"), m_folderEdt->text());
	if(fn.length() < 3)
		return;
    m_folderEdt->setText(fn);
    m_profile->setFolder(fn.toStdString());
    lsFilesInFolder();
    
}

void AFileDlg::lsFilesInFolder()
{
    QDir folderDir(tr(m_profile->_folder.c_str()));
    QStringList allFiles = folderDir.entryList(QDir::NoDotAndDotDot | QDir::Files);
    m_fileList->clear();
    
    QStringList::const_iterator it = allFiles.constBegin();
    for(;it!=allFiles.constEnd();++it) {
        if(it->endsWith(tr(m_profile->_ext.c_str())))
            m_fileList->addItem(*it);
    }
}

void AFileDlg::recvListItemSelection(const QPair<std::string, std::string> &x)
{
    m_profile->setName(x.second);
    const QString fn(m_profile->_name.c_str());
    m_nameEdt->setText(fn);
}

void AFileDlg::recvNameChange(const QString &x)
{ m_profile->setName(x.toStdString()); }

void AFileDlg::recvFolderChange(const QString &x)
{ m_profile->setFolder(x.toStdString()); }

}
