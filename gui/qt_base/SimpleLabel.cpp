#include "SimpleLabel.h"

namespace alo {

SimpleLabel::SimpleLabel(const QString &text, QWidget *parent) : QLabel(text, parent)
{
	setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    m_longText = m_shortText = text;
}

QSize SimpleLabel::minimumSizeHint() const
{
    return QSize(24, 24);
}

QSize SimpleLabel::sizeHint() const
{
    return QSize(120, 24);
}

void SimpleLabel::setLableText(bool shortened)
{
    if(shortened) setText(m_shortText);
    else setText(m_longText);
}

void SimpleLabel::setShortText(const QString &x)
{ m_shortText = x; }

}