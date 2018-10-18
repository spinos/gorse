#include "SimpleLabel.h"

namespace alo {

SimpleLabel::SimpleLabel(const QString &text, QWidget *parent) : QLabel(text, parent)
{
	setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
}

QSize SimpleLabel::minimumSizeHint() const
{
    return QSize(24, 24);
}

QSize SimpleLabel::sizeHint() const
{
    return QSize(120, 24);
}

}