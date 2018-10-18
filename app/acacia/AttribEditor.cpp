#include "AttribEditor.h"
#include "AcaciaScene.h"
#include <QVBoxLayout>
#include <QDebug>
#include <QSplitter>
#include <QSpacerItem>
#include <qt_graph/GlyphOps.h>
#include <qt_base/SimpleLabel.h>
#include <qt_base/FieldSlider.h>

using namespace alo;

AttribEditor::AttribEditor(AcaciaScene *scene, QWidget *parent) : QWidget(parent)
{
	m_scene = scene;
	leftBox = new QVBoxLayout;
	rightBox = new QVBoxLayout;
	leftBox->setSpacing(2);
    leftBox->setContentsMargins(2,2,2,2);
    rightBox->setSpacing(2);
    rightBox->setContentsMargins(2,2,2,2);
    QWidget *leftWiget = new QWidget;
    QWidget *rightWiget = new QWidget;
    m_leftSpace = new QSpacerItem(24,8, QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    m_rightSpace = new QSpacerItem(36,8, QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    leftBox->addItem(m_leftSpace);
    rightBox->addItem(m_rightSpace);
    
    leftWiget->setLayout(leftBox);
    rightWiget->setLayout(rightBox);

    QSplitter *splt = new QSplitter;
    splt->addWidget(leftWiget);
    splt->addWidget(rightWiget);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(2);
    mainLayout->setContentsMargins(2,2,2,2);
    mainLayout->addWidget(splt);
	setLayout(mainLayout);
}

void AttribEditor::recvSelectGlyph(bool isSelecting)
{
	if(isSelecting) {
		GlyphOps *ops = m_scene->getActiveOps();
		if(!ops)
			return;

		clearAttribs();
		lsAttribs(ops);
			
	} else {
		clearAttribs();
	}
}

void AttribEditor::clearAttribs()
{
	foreach (QWidget *widget, m_leftCollWigs)
        leftBox->removeWidget(widget);

    foreach (QWidget *widget, m_rightCollWigs)
        rightBox->removeWidget(widget);

	while (!m_leftCollWigs.isEmpty())
		delete m_leftCollWigs.dequeue();

	while (!m_rightCollWigs.isEmpty())
		delete m_rightCollWigs.dequeue();
	
	leftBox->removeItem(m_leftSpace);	
	rightBox->removeItem(m_rightSpace);
}

void AttribEditor::lsAttribs(GlyphOps *ops)
{
	std::map<std::string, QAttrib * >::iterator it = ops->attribBegin();
	for(;it!=ops->attribEnd();++it) {
		lsAttr(it->second);
	}

	foreach (QWidget *widget, m_leftCollWigs)
        leftBox->addWidget(widget);

    foreach (QWidget *widget, m_rightCollWigs)
        rightBox->addWidget(widget);

    leftBox->addItem(m_leftSpace);
    rightBox->addItem(m_rightSpace);
}

void AttribEditor::lsAttr(QAttrib *attr)
{
	switch(attr->attrType()) {
		case QAttrib::AtFloat :
			lsFloatAttr(attr);
		break;
		default :
		break;
	}
}

void AttribEditor::lsFloatAttr(alo::QAttrib *attr)
{
	SimpleLabel *lab = new SimpleLabel(QString(attr->label().c_str()));
	FieldSlider* sld = new FieldSlider();
	
    m_leftCollWigs.enqueue(lab);
    m_rightCollWigs.enqueue(sld);

	FloatAttrib *fattr = static_cast<FloatAttrib *>(attr);
	float val, val0, val1;
	fattr->getValue(val);
	fattr->getMin(val0);
	fattr->getMax(val1);
	sld->setLimit(val0, val1);
	sld->setValue(val);
	sld->setName(attr->attrName());
	
	connect(sld, SIGNAL(valueChanged(QPair<std::string, float>)),
            this, SLOT(recvFloatValue(QPair<std::string, float>)));
			
}

void AttribEditor::recvFloatValue(QPair<std::string, float> x)
{
	GlyphOps *ops = m_scene->getActiveOps();
	if(!ops)
		return;

	if(ops->setFloatAttrValue(x.first, x.second) )
		emit sendAttribChanged();
}
