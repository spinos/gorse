/*
 *  Attrib.cpp
 *  
 *
 *  Created by jian zhang on 8/6/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#include "Attrib.h"
#include <QPointF>

namespace alo {

QAttrib::QAttrib(const std::string &anm, AttrType atyp) :
m_connectable(0)
{ 
	m_name = m_label = anm;
	m_atyp = atyp;
}

QAttrib::~QAttrib()
{ 
	m_value.clear(); 
	if(m_connectable) delete m_connectable;
}

const std::string &QAttrib::attrName() const
{ return m_name; }

const QAttrib::AttrType &QAttrib::attrType() const
{ return m_atyp; }

QList<QVariant> &QAttrib::value()
{ return m_value; }

const QList<QVariant> &QAttrib::c_value() const
{ return m_value; }

const std::string &QAttrib::label() const
{ return m_label; }

const Connectable *QAttrib::connection() const
{ return m_connectable; }

void QAttrib::setLabel(const std::string &lb)
{ m_label = lb; }

void QAttrib::setConnectable(Connectable *x)
{ m_connectable = x; }

void QAttrib::SeparateAttrComponent(std::string &attrName, int &attrComponent)
{
	size_t found = attrName.find(".x"); 
	if(found != std::string::npos) {
		attrName.erase(found);
		attrComponent = 0;
		return;
	}
	found = attrName.find(".y"); 
	if(found != std::string::npos) {
		attrName.erase(found);
		attrComponent = 1;
		return;
	}
	found = attrName.find(".z"); 
	if(found != std::string::npos) {
		attrName.erase(found);
		attrComponent = 2;
		return;
	}
	attrComponent = -1;
}

BoolAttrib::BoolAttrib(const std::string &name) : QAttrib(name, AtBool)
{ value()<<false; }

void BoolAttrib::setValue(const bool& x)
{ value()[0] = x; }

void BoolAttrib::getValue(bool& y) const
{ y = c_value()[0].toBool(); }

IntAttrib::IntAttrib(const std::string &name) : QAttrib(name, AtInt)
{ value()<<0<<-999999<<999999; }

void IntAttrib::setValue(const int& x)
{ value()[0] = x; }

void IntAttrib::getValue(int& y) const
{ y = c_value()[0].toInt(); }

void IntAttrib::setMin(const int& x)
{ value()[1] = x; }

void IntAttrib::setMax(const int& x)
{ value()[2] = x; }

void IntAttrib::getMin(int& y) const
{ y = c_value()[1].toInt(); }

void IntAttrib::getMax(int& y) const
{ y = c_value()[2].toInt(); }

FloatAttrib::FloatAttrib(const std::string &name) : QAttrib(name, AtFloat)
{ value()<<0.f<<-999999.f<<999999.f; }

void FloatAttrib::setValue(const float& x)
{ value()[0] = x; }

void FloatAttrib::getValue(float& y) const
{ y = c_value()[0].toFloat(); }

void FloatAttrib::setMin(const float& x)
{ value()[1] = x; }

void FloatAttrib::setMax(const float& x)
{ value()[2] = x; }

void FloatAttrib::getMin(float& y) const
{ y = c_value()[1].toFloat(); }

void FloatAttrib::getMax(float& y) const
{ y = c_value()[2].toFloat(); }

Float2Attrib::Float2Attrib(const std::string &name) : QAttrib(name, AtFloat2)
{ value()<<0.f<<0.f<<-999999.f<<-999999.f<<999999.f<<999999.f; }

void Float2Attrib::setValue(const float &x, int component)
{ value()[component] = x; }

void Float2Attrib::setValue(const float *x)
{ 
	value()[0] = x[0]; 
	value()[1] = x[1]; 
}

void Float2Attrib::getValue(float *y) const
{ 
	y[0] = c_value()[0].toFloat(); 
	y[1] = c_value()[1].toFloat(); 
}

void Float2Attrib::setMin(const float* x)
{
	value()[2] = x[0];
	value()[3] = x[1];
}

void Float2Attrib::setMax(const float* x)
{
	value()[4] = x[0];
	value()[5] = x[1];
}
	
void Float2Attrib::getMin(float* y) const
{ 
	y[0] = c_value()[2].toFloat(); 
	y[1] = c_value()[3].toFloat(); 
}

void Float2Attrib::getMax(float* y) const
{ 
	y[0] = c_value()[4].toFloat(); 
	y[1] = c_value()[5].toFloat(); 
}

Float3Attrib::Float3Attrib(const std::string &name) : QAttrib(name, AtFloat3)
{ value()<<0.f<<0.f<<0.f; }

void Float3Attrib::setValue(const float *x)
{ 
	value()[0] = x[0]; 
	value()[1] = x[1]; 
	value()[2] = x[2]; 
}

void Float3Attrib::getValue(float *y) const
{ 
	y[0] = c_value()[0].toFloat(); 
	y[1] = c_value()[1].toFloat(); 
	y[2] = c_value()[2].toFloat(); 
}

Float4Attrib::Float4Attrib(const std::string &name) : QAttrib(name, AtFloat4)
{ value()<<0.f<<0.f<<0.f<<0.f; }

void Float4Attrib::setValue(const float *x)
{ 
	value()[0] = x[0]; 
	value()[1] = x[1]; 
	value()[2] = x[2]; 
	value()[3] = x[3]; 
}

void Float4Attrib::getValue(float *y) const
{ 
	y[0] = c_value()[0].toFloat(); 
	y[1] = c_value()[1].toFloat(); 
	y[2] = c_value()[2].toFloat(); 
	y[3] = c_value()[3].toFloat(); 
}

EnumAttrib::EnumAttrib(const std::string &name) : QAttrib(name, AtEnum)
{ value()<<0; }

EnumAttrib::~EnumAttrib()
{
	m_fields.clear();
}

void EnumAttrib::addField(const std::string &fieldName, int fieldValue)
{ 
	QPair<QString, int> b;
	b.first = QString(fieldName.c_str());
	b.second = fieldValue;
	m_fields<<b;
}

const int& EnumAttrib::numFields() const
{ return m_fields.size(); }

void EnumAttrib::getField(std::string &name, int &val, int i) const
{ 
	const QPair<QString, int> &b = m_fields[i];
	name = b.first.toStdString();
	val = b.second;
}

void EnumAttrib::setValue(const int& x)
{ value()[0] = x; }

void EnumAttrib::getValue(int& y) const
{ y = c_value()[0].toInt(); }


SplineAttrib::SplineAttrib(const std::string &name) : QAttrib(name, AtSpline)
{
	value()<<QPointF(1.f, 1.f)<<QPointF(.4f, 1.f)<<QPointF(.6f, 1.f);
}

void SplineAttrib::setSplineValue(float y0, float y1)
{
	value()[0] = QPointF(y0, y1);
}

void SplineAttrib::setSplineCv0(float x, float y)
{
	value()[1] = QPointF(x, y);
}

void SplineAttrib::setSplineCv1(float x, float y)
{
	value()[2] = QPointF(x, y);
}

void SplineAttrib::getSplineValue(float* y) const
{ 
	QPointF b = c_value()[0].toPointF();
	y[0] = b.x();
	y[1] = b.y();
}

void SplineAttrib::getSplineCv0(float* y) const
{
	QPointF b = c_value()[1].toPointF();
	y[0] = b.x();
	y[1] = b.y();
}

void SplineAttrib::getSplineCv1(float* y) const
{ 
	QPointF b = c_value()[2].toPointF();
	y[0] = b.x();
	y[1] = b.y();
}

StringAttrib::StringAttrib(const std::string &name, StrAttrType sat) : QAttrib(name, AtString)
{ 
	value()<<QString("");
	m_sat = sat;
}

void StringAttrib::setValue(const std::string& x)
{ value()[0] = QString(x.c_str()); }

void StringAttrib::getValue(std::string& y) const
{ 
	QString b = c_value()[0].toString();
	y = b.toStdString(); 
}

bool StringAttrib::isFileName() const
{ return m_sat == SaFileName; }

void StringAttrib::setStrAttrTyp(int x)
{ 
	if(x == SaFileName) m_sat = SaFileName; 
	if(x == SaDirectory) m_sat = SaDirectory; 
	if(x == SaNote) m_sat = SaNote; 
}

const StringAttrib::StrAttrType &StringAttrib::strAttrTyp() const
{ return m_sat; }

ActionAttrib::ActionAttrib(const std::string &name) : QAttrib(name, AtAction)
{ value()<<true<<QString(""); }

void ActionAttrib::setImageName(const std::string& x)
{ value()[1] = QString(x.c_str()); }

const std::string& ActionAttrib::imageName() const
{
	QString b = c_value()[1].toString();
	return b.toStdString(); 
}

void ActionAttrib::setValue(const bool &x)
{ value()[0] = x; }

void ActionAttrib::getValue(bool &y) const
{ y = c_value()[0].toBool(); }

MeshAttrib::MeshAttrib(const std::string &name) : QAttrib(name, AtMesh)
{ value()<<QVariant(); }
	
void MeshAttrib::setValue(AdaptableMesh *x)
{ value()[0].setValue(x); }

AdaptableMesh *MeshAttrib::getValue() const
{ return c_value()[0].value<AdaptableMesh*>(); }

ListAttrib::ListAttrib(const std::string &name) : QAttrib(name, AtList)
{ value()<<QString("unknown"); }

void ListAttrib::setValue(const std::string& x)
{ value()[0] = QString(x.c_str()); }

void ListAttrib::getValue(std::string& y) const
{ 
    QString b = c_value()[0].toString();
    y = b.toStdString(); 
}

void ListAttrib::setWidget(QListWidget *wig)
{ m_wig = wig; }

QListWidget *ListAttrib::widget()
{ return m_wig; }

}