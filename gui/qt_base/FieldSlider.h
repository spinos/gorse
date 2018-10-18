/* FieldSlider.h
 *
 */

#ifndef FIELDSLIDER_H
#define FIELDSLIDER_H

#include <QWidget>

namespace alo {

class FieldSlider : public QWidget
{
   Q_OBJECT

public:
    FieldSlider(QWidget *parent = 0);
    ~FieldSlider();
    
    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;

private slots:
    void ShowContextMenu(const QPoint& pos);
    
private:
   void paintEvent(QPaintEvent *e) override;

private:
   float m_var;
   int m_lastPos;
   int m_startPos;
   int m_stepInd;
   static const char *SMenuLabels[];
   static const float SFSteps[];
};

}

#endif // FIELDSLIDER_H