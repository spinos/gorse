#ifndef ALO_CHECK_LABEL_H
#define ALO_CHECK_LABEL_H

#include <QCheckBox>

namespace alo {

class CheckLabel : public QCheckBox {

	Q_OBJECT

public:
	CheckLabel(QWidget *parent = 0);
	
	void setValue(bool x);
    void setName(const std::string &name);

	QSize minimumSizeHint() const override;
    QSize sizeHint() const override;
    
protected:
    virtual void mousePressEvent(QMouseEvent *e) override;
    virtual void mouseReleaseEvent(QMouseEvent *e) override;
    
signals :
    void valueChanged(QPair<std::string, bool>);
    void beginEditing();
    void endEditing();
    
private slots:
    void notifyValueChange();
    
private:
    std::string m_name;
    bool m_val;

};

}

#endif