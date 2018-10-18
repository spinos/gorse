/* FieldSlider.h
 *
 */

#ifndef FIELDSLIDER_H
#define FIELDSLIDER_H

#include <QWidget>
#include <string>

namespace alo {

class FieldSlider : public QWidget
{
   Q_OBJECT

public:
    FieldSlider(QWidget *parent = 0);
    ~FieldSlider();
    
    void setValue(float x);
    void setLimit(float mn, float mx);
    void setName(const std::string &name);
    
    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    
signals:
    void valueChanged(QPair<std::string, float>);

private slots:
    void ShowContextMenu(const QPoint& pos);
    
private:
   void paintEvent(QPaintEvent *e) override;

private:
    std::string m_name;
   float m_var;
   float m_preVar;
   float m_limit[2];
   int m_lastPos;
   int m_startPos;
   int m_stepInd;
   static const char *SMenuLabels[];
   static const float SFSteps[];
};

}

#endif // FIELDSLIDER_H