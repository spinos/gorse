#include "CheckLabel.h"

namespace alo {

CheckLabel::CheckLabel(QWidget *parent) : QCheckBox(parent)
{
    connect(this, SIGNAL(clicked()),
    this, SLOT(notifyValueChange()));
	setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
}

void CheckLabel::setValue(bool x)
{ 
    m_val = x; 
    setChecked(x);
}

void CheckLabel::setName(const std::string &name)
{ m_name = name; }

QSize CheckLabel::minimumSizeHint() const
{
    return QSize(24, 24);
}

QSize CheckLabel::sizeHint() const
{
    return QSize(120, 24);
}

void CheckLabel::notifyValueChange()
{
    bool b = isChecked();
    if(b!=m_val) {
        m_val = b;
        QPair<std::string, bool> sigv(m_name, m_val);
        emit valueChanged(sigv);
    }
}

void CheckLabel::mousePressEvent(QMouseEvent *e)
{ 
    QCheckBox::mousePressEvent(e);
    emit beginEditing(); 
}

void CheckLabel::mouseReleaseEvent(QMouseEvent *e)
{ 
    QCheckBox::mouseReleaseEvent(e);
    emit endEditing(); 
}

}
