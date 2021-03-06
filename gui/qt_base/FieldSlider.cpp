#include "FieldSlider.h"
#include <QPainter>
#include <QMouseEvent>
#include <QMenu>
#include "ImageCollector.h"

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
    m_preVar = m_var = 1.f; 
    m_limit[0] = -1e10f;
    m_limit[1] = 1e10f;
    m_lastPos = -1;
    m_startPos = -1;
    m_stepInd = 1;
    m_isContinuous = false;
    m_useIntSteps = false;
    setContextMenuPolicy(Qt::CustomContextMenu);
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    connect(this, &FieldSlider::customContextMenuRequested,
            this, &FieldSlider::showContextMenu );
#else
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
    this, SLOT(ShowContextMenu(const QPoint&)));
#endif
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
}

FieldSlider::~FieldSlider()
{}

void FieldSlider::setValue(float x)
{ m_var = x; }

void FieldSlider::setLimit(float mn, float mx)
{ m_limit[0] = mn; m_limit[1] = mx; }

void FieldSlider::setName(const std::string &name)
{ m_name = name; }

void FieldSlider::setUseIntSteps(bool x)
{ 
    m_useIntSteps = x; 
    if(x) m_stepInd = 3;
    else m_stepInd = 1;
}

QSize FieldSlider::minimumSizeHint() const
{
    return QSize(36, 24);
}

QSize FieldSlider::sizeHint() const
{
    return QSize(72, 24);
}

void FieldSlider::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setPen(Qt::black);
                //painter.setBrush(QColor(191, 215, 255));
    QPixmap headpix = ImageCollector::CollectPixmap(":images/field_head.png");
	QPixmap bodypix = ImageCollector::CollectPixmap(":images/field_body.png");
	QPixmap tailpix = ImageCollector::CollectPixmap(":images/field_tail.png");
    QPixmap groovepix = ImageCollector::CollectPixmap(":images/field_groove.png");        
    
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
    
    if(m_useIntSteps) 
        painter.drawText(QPoint(10, 16), QString("%1").arg(m_var, 0, 'f', 0));
    else 
        painter.drawText(QPoint(10, 16), QString("%1").arg(m_var, 0, 'f', 3));
}

void FieldSlider::mousePressEvent(QMouseEvent *e)
{
    m_startPos = m_lastPos = e->pos().x();
    update();
    emit beginEditing();
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

    if(m_var < m_limit[0]) m_var = m_limit[0];
    if(m_var > m_limit[1]) m_var = m_limit[1];
    
    m_lastPos = px;
    update();
    if(m_isContinuous) notifyValueChange();
}

void FieldSlider::mouseReleaseEvent(QMouseEvent *)
{
    m_startPos = m_lastPos = -1;
    update();
    notifyValueChange();
    emit endEditing();
}

void FieldSlider::notifyValueChange()
{
    if(m_preVar != m_var) {
        m_preVar = m_var;
        QPair<std::string, float> sigv(m_name, m_var);
        emit valueChanged(sigv);
    }
}

void FieldSlider::showContextMenu(const QPoint& pos)
{
    QPoint globalPos = mapToGlobal(pos);
    // for QAbstractScrollArea and derived classes you would use:
    // QPoint globalPos = myWidget->viewport()->mapToGlobal(pos);
    int menuBegin = m_useIntSteps ? 3 : 0;

    QMenu myMenu;
    for(int i=menuBegin;i<7;++i) {
        QAction * a = myMenu.addAction(tr(SMenuLabels[i]));
        a->setCheckable(true);
        a->setData(QVariant(i));
        if(m_stepInd == i) a->setChecked(true);
    }
    QAction * ac = myMenu.addAction(tr("Continuous"));
    ac->setData(QVariant(8));
    ac->setCheckable(true);
    ac->setChecked(m_isContinuous);

    QAction* selectedItem = myMenu.exec(globalPos);
    if (selectedItem) {
        int k = selectedItem->data().toInt();
        if(k < 8)
            m_stepInd = k;
        else 
            m_isContinuous = !m_isContinuous;
    }
}

}
