#include "AttribEditor.h"
#include <qt_graph/GlyphScene.h>
#include <QVBoxLayout>
#include <QDebug>
#include <QSplitter>
#include <QSpacerItem>
#include <QScrollArea>
#include <QListWidget>
#include <qt_graph/GlyphOps.h>
#include <qt_base/SimpleLabel.h>
#include <qt_base/FieldSlider.h>
#include <qt_base/ListSelector.h>
#include <qt_base/CheckLabel.h>
#include <qt_base/StringEdit.h>

using namespace alo;

AttribEditor::AttribEditor(alo::GlyphScene *scene, QWidget *parent) : QWidget(parent)
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
    m_leftSpace = new QSpacerItem(36,8, QSizePolicy::Preferred, QSizePolicy::Expanding);
    m_rightSpace = new QSpacerItem(36,8, QSizePolicy::Preferred, QSizePolicy::Expanding);
    leftBox->addItem(m_leftSpace);
    rightBox->addItem(m_rightSpace);
    
    leftWiget->setLayout(leftBox);
    rightWiget->setLayout(rightBox);

    QSplitter *splt = new QSplitter;
    splt->addWidget(leftWiget);
    splt->addWidget(rightWiget);

    QScrollArea *scroll = new QScrollArea;
	scroll->setWidgetResizable(true);
	
	scroll->setWidget(splt);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(2);
    mainLayout->setContentsMargins(2,2,2,2);
    mainLayout->addWidget(scroll);
	setLayout(mainLayout);
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)    
    connect(splt, &QSplitter::splitterMoved,
            this, &AttribEditor::recvSplitMove );
#endif
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

    ops->postUI();
}

void AttribEditor::lsAttr(QAttrib *attr)
{
	switch(attr->attrType()) {
		case QAttrib::AtFloat :
			lsFloatAttr(attr);
		break;
		case QAttrib::AtFloat2 :
			lsFloat2Attr(attr);
		break;
        case QAttrib::AtFloat3 :
			lsFloat3Attr(attr);
		break;
		case QAttrib::AtList :
			lsListAttr(attr);
		break;
		case QAttrib::AtBool :
			lsBoolAttr(attr);
		break;
		case QAttrib::AtInt :
			lsIntAttr(attr);
		break;
		case QAttrib::AtString :
			lsStringAttr(attr);
		break;
		default :
		break;
	}
}

void AttribEditor::lsIntAttr(alo::QAttrib *attr)
{
	SimpleLabel *lab = new SimpleLabel(QString(attr->label().c_str()));
    lab->setShortText(QString(attr->attrName().c_str()));
	FieldSlider* sld = new FieldSlider();
	
    m_leftCollWigs.enqueue(lab);
    m_rightCollWigs.enqueue(sld);

	IntAttrib *fattr = static_cast<IntAttrib *>(attr);
	int val, val0, val1;
	fattr->getValue(val);
	fattr->getMin(val0);
	fattr->getMax(val1);
	sld->setLimit(val0, val1);
	sld->setValue(val);
	sld->setName(attr->attrName());
	sld->setUseIntSteps(true);

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
	connect(sld, &FieldSlider::valueChanged,
            this, &AttribEditor::recvIntValue);
    connect(sld, &FieldSlider::beginEditing,
            this, &AttribEditor::recvBeginEditing);
    connect(sld, &FieldSlider::endEditing,
            this, &AttribEditor::recvEndEditing);
#else	
	connect(sld, SIGNAL(valueChanged(QPair<std::string, float>)),
            this, SLOT(recvIntValue(QPair<std::string, float>)));
#endif			
}

void AttribEditor::lsFloatAttr(alo::QAttrib *attr)
{
	SimpleLabel *lab = new SimpleLabel(QString(attr->label().c_str()));
    lab->setShortText(QString(attr->attrName().c_str()));
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
	
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
	connect(sld, &FieldSlider::valueChanged,
            this, &AttribEditor::recvFloatValue);
    connect(sld, &FieldSlider::beginEditing,
            this, &AttribEditor::recvBeginEditing);
    connect(sld, &FieldSlider::endEditing,
            this, &AttribEditor::recvEndEditing);
#else	
	connect(sld, SIGNAL(valueChanged(QPair<std::string, float>)),
            this, SLOT(recvFloatValue(QPair<std::string, float>)));
#endif			
}

void AttribEditor::lsFloat2Attr(alo::QAttrib *attr)
{
	SimpleLabel *lab = new SimpleLabel(QString(attr->label().c_str()));
    lab->setShortText(QString(attr->attrName().c_str()));
	FieldSlider* xsld = new FieldSlider();
	FieldSlider* ysld = new FieldSlider();
	
    m_leftCollWigs.enqueue(lab);
    m_leftCollWigs.enqueue(new SimpleLabel(tr("")));
    m_rightCollWigs.enqueue(xsld);
    m_rightCollWigs.enqueue(ysld);

	Float2Attrib *fattr = static_cast<Float2Attrib *>(attr);
	float val[2], val0[2], val1[2];
	fattr->getValue(val);
	fattr->getMin(val0);
	fattr->getMax(val1);
	xsld->setLimit(val0[0], val1[0]);
	xsld->setValue(val[0]);
	xsld->setName(attr->attrName() + ".x");
	ysld->setLimit(val0[1], val1[1]);
	ysld->setValue(val[1]);
	ysld->setName(attr->attrName() + ".y");
	
	connect(xsld, SIGNAL(valueChanged(QPair<std::string, float>)),
           this, SLOT(recvFloatValue(QPair<std::string, float>)));
	connect(ysld, SIGNAL(valueChanged(QPair<std::string, float>)),
           this, SLOT(recvFloatValue(QPair<std::string, float>)));
			
}

void AttribEditor::lsFloat3Attr(alo::QAttrib *attr)
{
	SimpleLabel *lab = new SimpleLabel(QString(attr->label().c_str()));
    lab->setShortText(QString(attr->attrName().c_str()));
	FieldSlider* xsld = new FieldSlider();
	FieldSlider* ysld = new FieldSlider();
    FieldSlider* zsld = new FieldSlider();
	
    m_leftCollWigs.enqueue(lab);
    m_leftCollWigs.enqueue(new SimpleLabel(tr("")));
    m_leftCollWigs.enqueue(new SimpleLabel(tr("")));
    m_rightCollWigs.enqueue(xsld);
    m_rightCollWigs.enqueue(ysld);
    m_rightCollWigs.enqueue(zsld);

	Float3Attrib *fattr = static_cast<Float3Attrib *>(attr);
	float val[3], val0[3], val1[3];
	fattr->getValue(val);
	fattr->getMin(val0);
	fattr->getMax(val1);
	xsld->setLimit(val0[0], val1[0]);
	xsld->setValue(val[0]);
	xsld->setName(attr->attrName() + ".x");
	ysld->setLimit(val0[1], val1[1]);
	ysld->setValue(val[1]);
	ysld->setName(attr->attrName() + ".y");
    zsld->setLimit(val0[2], val1[2]);
	zsld->setValue(val[2]);
	zsld->setName(attr->attrName() + ".z");
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)   	
	connect(xsld, &FieldSlider::valueChanged,
           this, &AttribEditor::recvFloatValue);
	connect(ysld, &FieldSlider::valueChanged,
           this, &AttribEditor::recvFloatValue);
    connect(zsld, &FieldSlider::valueChanged,
           this, &AttribEditor::recvFloatValue);       
#endif
}

void AttribEditor::lsListAttr(alo::QAttrib *attr)
{
	SimpleLabel *lab = new SimpleLabel(QString(attr->label().c_str()));
	lab->setShortText(QString(attr->attrName().c_str()));
	m_leftCollWigs.enqueue(lab);
    m_leftCollWigs.enqueue(new SimpleLabel(tr("")));
    m_leftCollWigs.enqueue(new SimpleLabel(tr("")));
    
    ListSelector *lg = new ListSelector();
    lg->setName(attr->attrName() );
    lg->setFixedHeight(78);
    m_rightCollWigs.enqueue(lg);

    ListAttrib *flist = static_cast<ListAttrib *>(attr);
    flist->setWidget(lg);

    connect(lg, SIGNAL(selectionChanged(QPair<std::string, std::string>)),
           this, SLOT(recvListItemSelection(QPair<std::string, std::string>) ) );
}

void AttribEditor::lsBoolAttr(alo::QAttrib *attr)
{
    SimpleLabel *lab = new SimpleLabel(QString(attr->label().c_str()));
    lab->setShortText(QString(attr->attrName().c_str()));
	CheckLabel* chk = new CheckLabel();
	
    m_leftCollWigs.enqueue(lab);
    m_rightCollWigs.enqueue(chk);

	BoolAttrib *fattr = static_cast<BoolAttrib *>(attr);
	bool val;
	fattr->getValue(val);
	
	chk->setValue(val);
	chk->setName(attr->attrName());
	
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
	connect(chk, &CheckLabel::valueChanged,
            this, &AttribEditor::recvBoolValue);
    connect(chk, &CheckLabel::beginEditing,
            this, &AttribEditor::recvBeginEditing);
    connect(chk, &CheckLabel::endEditing,
            this, &AttribEditor::recvEndEditing);
#else	
	connect(chk, SIGNAL(valueChanged(QPair<std::string, bool>)),
           this, SLOT(recvBoolValue(QPair<std::string, bool>)));
#endif  
}

void AttribEditor::lsStringAttr(alo::QAttrib *attr)
{
	SimpleLabel *lab = new SimpleLabel(QString(attr->label().c_str()));
    lab->setShortText(QString(attr->attrName().c_str()));
	StringEdit *fld = new StringEdit();
/// open only
    GlyphOps *ops = m_scene->getActiveOps();
    AFileDlgProfile *rp = ops->readFileProfileR();
    if(rp) fld->setReadFileProfile(rp);
    
	m_leftCollWigs.enqueue(lab);
    m_rightCollWigs.enqueue(fld);

    StringAttrib *fattr = static_cast<StringAttrib *>(attr);
    std::string val;
	fattr->getValue(val);
	
	fld->setValue(val);
	fld->setName(attr->attrName());

	connect(fld, SIGNAL(valueChanged(QPair<std::string, std::string>)),
           this, SLOT(recvStringValue(QPair<std::string, std::string>)));

}

void AttribEditor::recvIntValue(QPair<std::string, float> x)
{
	GlyphOps *ops = m_scene->getActiveOps();
	if(!ops) return;

	if(ops->setIntAttrValue(x.first, x.second) )
		emit sendAttribChanged();
}

void AttribEditor::recvFloatValue(QPair<std::string, float> x)
{
	GlyphOps *ops = m_scene->getActiveOps();
	if(!ops) return;

	if(ops->setFloatAttrValue(x.first, x.second) )
		emit sendAttribChanged();
}

void AttribEditor::recvListItemSelection(QPair<std::string, std::string> x)
{
	GlyphOps *ops = m_scene->getActiveOps();
	if(!ops) return;

	if(ops->setListAttrValue(x.first, x.second) )
		emit sendAttribChanged();
}

void AttribEditor::recvBoolValue(QPair<std::string, bool> x)
{
    GlyphOps *ops = m_scene->getActiveOps();
	if(!ops) return;

	if(ops->setBoolAttrValue(x.first, x.second) )
		emit sendAttribChanged();
}

void AttribEditor::recvStringValue(QPair<std::string, std::string> x)
{
    GlyphOps *ops = m_scene->getActiveOps();
	if(!ops) return;

	if(ops->setStringAttrValue(x.first, x.second) )
		emit sendAttribChanged();
}

void AttribEditor::recvSplitMove(int pos, int index)
{
    bool isShort = pos < 50;
    foreach (QWidget *widget, m_leftCollWigs) {
        SimpleLabel *lab = dynamic_cast<SimpleLabel *>(widget);
        if(lab) lab->setLableText(isShort);
    }
}

void AttribEditor::recvBeginEditing()
{ emit sendAttribChanged(); }

void AttribEditor::recvEndEditing()
{ emit sendAttribChanged(); }

