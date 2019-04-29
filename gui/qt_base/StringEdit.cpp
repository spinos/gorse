#include "StringEdit.h"
#include <QLineEdit>
#include <QHBoxLayout>
#include <QPushButton>
#include <QIcon>
#include <QFileDialog>
#include <QDebug>
#include "AFileDlg.h"
#include "ImageCollector.h"

namespace alo {

StringEdit::StringEdit(QWidget *parent) : QWidget(parent),
m_readFileProfile(0)
{
    m_edit = new QLineEdit(this);
    m_btn = new QPushButton(this);
    QPixmap openpix = ImageCollector::CollectPixmap(":images/open_file_small.png");
    m_btn->setIcon(QIcon(openpix) );
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(m_btn);
    layout->addWidget(m_edit);
    layout->setContentsMargins(0,0,0,0);
    setLayout(layout);

    connect(m_btn, SIGNAL(clicked()),
    this, SLOT(selectFile()));
    connect(m_edit, SIGNAL(textEdited(QString)),
    this, SLOT(notifyValueChange(QString)));
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
}

void StringEdit::setValue(const std::string &x)
{ 
    m_val = x; 
    m_edit->setText(tr(x.c_str()));
}

void StringEdit::setName(const std::string &name)
{ m_name = name; }

void StringEdit::setReadFileProfile(AFileDlgProfile *prof)
{ m_readFileProfile = prof; }

QSize StringEdit::minimumSizeHint() const
{
    return QSize(24, 24);
}

QSize StringEdit::sizeHint() const
{
    return QSize(120, 24);
}

void StringEdit::notifyValueChange(const QString &t)
{
    m_val = t.toStdString();
    QPair<std::string, std::string> sigv(m_name, m_val);
    emit valueChanged(sigv);
}

void StringEdit::selectFile()
{
    QString fn;
    if(m_readFileProfile) {
        AFileDlg d(*m_readFileProfile, this);
        int res = d.exec();
        if(res == QDialog::Rejected) {
            qDebug()<<"abort open file";
            return;
        }
        fn = tr(m_readFileProfile->getFilePath().c_str());
        
    } else
        fn = QFileDialog::getOpenFileName(this, tr("Open File"));
    
    if(fn.size() < 3) return;

    if(fn != tr(m_val.c_str()) ) {
        m_edit->setText(fn);
        notifyValueChange(fn);
    }
}

}
