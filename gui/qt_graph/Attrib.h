/*
 *  Attrib.h
 *  attribute name type value
 *
 *  Created by jian zhang on 8/6/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_Q_ATTRIB_H
#define ALO_Q_ATTRIB_H

#include <string>
#include <QVariant>
#include <geom/AdaptableMesh.h>

QT_BEGIN_NAMESPACE
class QListWidget;
QT_END_NAMESPACE

namespace alo {

enum ConnectableSide {
	ConnectLeft = 1,
	ConnectRight,
	ConnectTop,
	ConnectBottom
};

struct Connectable {
	bool _isOutgoing;
	int _side;
	int _px, _py;
};

class QAttrib {

	std::string m_name;
	std::string m_label;
	QList<QVariant> m_value;
	Connectable *m_connectable;
	
public:
	enum AttrType {
		AtUnknown = 0,
		AtBool,
		AtInt,
		AtFloat,
		AtFloat2,
		AtFloat3,
		AtFloat4,
		AtEnum,
		AtSpline = 8,
		AtString,
		AtAction,
		AtMesh,
		AtList
	};

	QAttrib(const std::string &anm, AttrType atyp);
	virtual ~QAttrib();
	
	const std::string &attrName() const;
	const AttrType &attrType() const;
	const std::string &label() const;
	const Connectable *connection() const;

	void setLabel(const std::string &lb);
	void setConnectable(Connectable *x);

	static void SeparateAttrComponent(std::string &attrName, int &attrComponent);

protected:

	QList<QVariant> &value();
	const QList<QVariant> &c_value() const;

private:

	AttrType m_atyp;
	
};

class BoolAttrib : public QAttrib {

public:
	BoolAttrib(const std::string &name);

	void setValue(const bool& x);
	void getValue(bool& y) const;

private:
};

class IntAttrib : public QAttrib {

public:
	IntAttrib(const std::string &name);

	void setValue(const int& x);
	void getValue(int& y) const;

	void setMin(const int& x);
	void setMax(const int& x);
	
	void getMin(int& y) const;
	void getMax(int& y) const;
	
	
private:
};

class FloatAttrib : public QAttrib {

public:
	FloatAttrib(const std::string &name);

	void setValue(const float& x);
	void getValue(float& y) const;

	void setMin(const float& x);
	void setMax(const float& x);
	
	void getMin(float& y) const;
	void getMax(float& y) const;
	
	
private:
};

class Float2Attrib : public QAttrib {

public:
	Float2Attrib(const std::string &name);

	void setValue(const float &x, int component);

	void setValue(const float *x);
	void getValue(float *y) const;

	void setMin(const float* x);
	void setMax(const float* x);
	
	void getMin(float* y) const;
	void getMax(float* y) const;
	
private:
};

class Float3Attrib : public QAttrib {

public:
	Float3Attrib(const std::string &name);

	void setValue(const float *x);
	void getValue(float *y) const;
	
private:
};

class Float4Attrib : public QAttrib {

public:
	Float4Attrib(const std::string &name);

	void setValue(const float *x);
	void getValue(float *y) const;
	
private:
};

class EnumAttrib : public QAttrib {

	QList<QPair<QString, int > > m_fields;
	
public:
	EnumAttrib(const std::string &name);
	virtual ~EnumAttrib();
	
	void addField(const std::string &fieldName, int fieldValue);
	const int& numFields() const;

	void getField(std::string &name, int &val, int i) const;
	
	void setValue(const int& x);
	void getValue(int& y) const;
};

class SplineAttrib : public QAttrib {

	float m_splineValue[2];
/// cv0 cv1
	float m_splineCv[4];
	
public:
	SplineAttrib(const std::string &name);
	
	void setSplineValue(float y0, float y1);
	void setSplineCv0(float x, float y);
	void setSplineCv1(float x, float y);
	
	void getSplineValue(float* y) const;
	void getSplineCv0(float* y) const;
	void getSplineCv1(float* y) const;
	
};

class StringAttrib : public QAttrib {
	
public:
	enum StrAttrType {
		SaUnknown = 512,
		SaFileName,
		SaDirectory,
		SaNote
	};

	StringAttrib(const std::string &name, StrAttrType sat = SaUnknown);

	void setStrAttrTyp(int x);
	const StrAttrType &strAttrTyp() const;
	
	void setValue(const std::string& x);
	void getValue(std::string& y) const;
	
	bool isFileName() const;
	
private:
	StrAttrType m_sat;
};

class ActionAttrib : public QAttrib {
	
public:
	ActionAttrib(const std::string &name);
	
	void setImageName(const std::string& x);
	const std::string& imageName() const;

	void setValue(const bool &x);
	void getValue(bool &y) const;
	
};

Q_DECLARE_METATYPE(AdaptableMesh*)

class MeshAttrib : public QAttrib {
	
public:
	MeshAttrib(const std::string &name);
	
	void setValue(AdaptableMesh *x);
	AdaptableMesh *getValue() const;
	
};

class ListAttrib : public QAttrib {
	
	QListWidget *m_wig;

public:
	ListAttrib(const std::string &name);
	
	void setValue(const std::string& x);
	void getValue(std::string& y) const;
	
	void setWidget(QListWidget *wig);
    QListWidget *widget();
	
};

}
#endif