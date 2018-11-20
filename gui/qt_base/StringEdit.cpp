#include "StringEdit.h"
#include <QLineEdit>
#include <QHBoxLayout>
#include <QPushButton>
#include <QIcon>
#include <QFileDialog>
#include "ImageCollector.h"

namespace alo {

StringEdit::StringEdit(QWidget *parent) : QWidget(parent)
{
    m_edit = new QLineEdit(this);
    m_btn = new QPushButton(this);
    QPixmap openpix = ImageCollector::CollectPixmap(":images/open_file_small.png");
    m_btn->setIcon(QIcon(openpix) );
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(m_edit);
    layout->addWidget(m_btn);
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
    QString fn = QFileDialog::getOpenFileName(this, tr("Open File"));
    if(fn.size() < 1)
        return;

    if(fn != tr(m_val.c_str()) ) {
        m_edit->setText(fn);
        notifyValueChange(fn);
    }
}

}
