#ifndef ALO_STRING_EDIT_H
#define ALO_STRING_EDIT_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QPushButton;
QT_END_NAMESPACE

namespace alo {

class StringEdit : public QWidget {

	Q_OBJECT

public:
	StringEdit(QWidget *parent = 0);
	
	void setValue(const std::string &x);
    void setName(const std::string &name);

	QSize minimumSizeHint() const override;
    QSize sizeHint() const override;
    
signals :
    void valueChanged(QPair<std::string, std::string>);
    
private slots:
    void notifyValueChange(const QString &t);
    void selectFile();
    
private:
    QLineEdit *m_edit;
    QPushButton *m_btn;
    std::string m_name;
    std::string m_val;

};

}

#endif
