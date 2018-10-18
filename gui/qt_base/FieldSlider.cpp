#include "FieldSlider.h"
#include <QPainter>
#include <QMouseEvent>
#include <QMenu>
#include <QPixmapCache>

namespace alo {

const char *FieldSlider::SMenuLabels[] = {
".001",
".01",
".1",
"1",
"10",
"100",
"1000"
};
const float FieldSlider::SFSteps[] = {
.001f, 
.01f, 
.1f, 
1.f, 
10.f, 
100.f,
1000.f
};

FieldSlider::FieldSlider(QWidget *parent)
   : QWidget(parent)
{ 
    m_var = 42.f; 
    m_lastPos = -1;
    m_startPos = -1;
    m_stepInd = 1;
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
    this, SLOT(ShowContextMenu(const QPoint&)));
}

FieldSlider::~FieldSlider()
{}

QSize FieldSlider::minimumSizeHint() const
{
    return QSize(72, 24);
}
//! [1]

//! [2]
QSize FieldSlider::sizeHint() const
{
    return QSize(120, 24);
}
/*
QPixmap cached1(const QString &img)
{
    if (QPixmap *p = QPixmapCache::find(img))
        return *p;

    QPixmap pm;
    pm = QPixmap::fromImage(QImage(img), Qt::OrderedDither | Qt::OrderedAlphaDither);
    if (pm.isNull())
        return QPixmap();

    QPixmapCache::insert(img, pm);
    return pm;
}*/

void FieldSlider::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setPen(Qt::black);
                //painter.setBrush(QColor(191, 215, 255));
    QPixmap headpix("field_head.png");
	QPixmap bodypix("field_body.png");
	QPixmap tailpix("field_tail.png");
    QPixmap groovepix("field_groove.png");        
    
    painter.drawPixmap(QRect(13, 0, width() - 26, 24), bodypix);
    
    if(m_startPos > 0) {       
        int gs = m_lastPos - m_startPos;
        int ags = gs;
        if(ags < 0) ags = -ags;
        if(ags < 1) ags = 1;
        if(gs > -1)
            painter.drawPixmap(QRect(m_startPos, 0, ags, 24), groovepix);
        else
            painter.drawPixmap(QRect(m_lastPos, 0, ags, 24), groovepix);
    }
    
    painter.drawPixmap(QRect(0, 0, 13, 24), headpix);
  
    painter.drawPixmap(QRect(width() - 13, 0, 13, 24), tailpix);
    
    painter.drawText(QPoint(10, 16), QString("%1").arg(m_var, 0, 'f', 3));
}

void FieldSlider::mousePressEvent(QMouseEvent *e)
{
    m_startPos = m_lastPos = e->pos().x();
    update();
}

void FieldSlider::mouseMoveEvent(QMouseEvent *e)
{
    int px = e->pos().x();
    int dx = px - m_lastPos;
    m_var += SFSteps[m_stepInd] * dx;

    if(m_stepInd > 0) {
        const float &r = SFSteps[m_stepInd];
        int ir = m_var / r;
        m_var = r * ir;
    }

    m_lastPos = px;
    update();
}

void FieldSlider::mouseReleaseEvent(QMouseEvent *)
{
    m_startPos = m_lastPos = -1;
    update();
}

void FieldSlider::ShowContextMenu(const QPoint& pos)
{
    QPoint globalPos = mapToGlobal(pos);
    // for QAbstractScrollArea and derived classes you would use:
    // QPoint globalPos = myWidget->viewport()->mapToGlobal(pos);

    QMenu myMenu;
    for(int i=0;i<7;++i) {
        QAction * a = myMenu.addAction(tr(SMenuLabels[i]));
        a->setCheckable(true);
        a->setData(QVariant(i));
        if(m_stepInd == i) a->setChecked(true);
    }

    QAction* selectedItem = myMenu.exec(globalPos);
    if (selectedItem) {
        m_stepInd = selectedItem->data().toInt();
    }

}

}
