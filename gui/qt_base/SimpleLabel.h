#ifndef ALO_SIMPLE_LABEL_H
#define ALO_SIMPLE_LABEL_H

#include <QLabel>

namespace alo {

class SimpleLabel : public QLabel {

	Q_OBJECT

public:
	SimpleLabel(const QString &text, QWidget *parent = 0);

	QSize minimumSizeHint() const override;
    QSize sizeHint() const override;
    
    void setLableText(bool shortened);
    void setShortText(const QString &x);
    
private:
    QString m_longText;
    QString m_shortText;

};

}

#endif