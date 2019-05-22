/*
 *  AttribCreator.cpp
 *  gorse
 *
 *  2019/5/22
 */

#include "AttribCreator.h"
#include "Attrib.h"
#include "GlyphOps.h"
#include <QFile>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>

namespace alo {

QJsonObject AttribCreator::attributePresetObj;

void AttribCreator::loadAttributePreset(const QString &fileName)
{
	QFile loadFile(fileName);
	if (!loadFile.open(QIODevice::ReadOnly) ) {
		qWarning("Couldn't open file for attribute preset");
		return;
	}

	QByteArray loadData = loadFile.readAll();
	QJsonDocument loadDoc(QJsonDocument::fromJson(loadData));
	
	attributePresetObj = loadDoc.object();
}

void AttribCreator::addAttributes(GlyphOps *ops, const QJsonObject &content)
{
	QJsonArray attrArray = content["attribs"].toArray();
    for(int i=0; i< attrArray.size();++i) {
    	QJsonObject attrObject = attrArray[i].toObject();
		addAttribute(ops, attrObject);
	}
}

void AttribCreator::addAttribute(GlyphOps *ops, const QJsonObject &content)
{
	const int typ = content["typ"].toInt();
	QAttrib *b = 0;
	switch(typ) {
		case QAttrib::AtBool :
			b = createBoolAttribute(content);
			break;
		case QAttrib::AtInt :
			b = createIntAttribute(content);
			break;
		case QAttrib::AtFloat :
			b = createFloatAttribute(content);
			break;
		case QAttrib::AtFloat2 :
			b = createFloat2Attribute(content);
			break;
        case QAttrib::AtFloat3 :
			b = createFloat3Attribute(content);
			break;
		case QAttrib::AtMesh :
			b = createMeshAttribute(content);
			break;
		case QAttrib::AtList :
			b = createListAttribute(content);
			break;
		case QAttrib::AtString :
			b = createStringAttribute(content);
			break;
		case QAttrib::AtTransformSet :
			addTransformAttributes(ops);
			break;
		default:
			break;
	}

	if(b) {
		b->setLabel(content["label"].toString().toStdString());
		addConnection(b, content);

		ops->addAttribute(b);
	}
}

QAttrib *AttribCreator::createBoolAttribute(const QJsonObject &content)
{
	QString name = content["name"].toString();
	std::string snm = name.toStdString();
	BoolAttrib *b = new BoolAttrib(snm);
	b->setValue(content["value"].toBool());
	return b;
}

QAttrib *AttribCreator::createIntAttribute(const QJsonObject &content)
{
	QString name = content["name"].toString();
	std::string snm = name.toStdString();
	IntAttrib *b = new IntAttrib(snm);
	b->setValue(content["value"].toInt());
	b->setMin(content["min"].toInt());
	b->setMax(content["max"].toInt());
	return b;
}

QAttrib *AttribCreator::createFloatAttribute(const QJsonObject &content)
{
	QString name = content["name"].toString();
	std::string snm = name.toStdString();
	FloatAttrib *b = new FloatAttrib(snm);
	float fv = content["value"].toDouble();
	b->setValue(fv);
	float fm = content["min"].toDouble();
	b->setMin(fm);
	float fx = content["max"].toDouble();
	b->setMax(fx);
	return b;
}

QAttrib *AttribCreator::createFloat2Attribute(const QJsonObject &content)
{
	QString name = content["name"].toString();
	std::string snm = name.toStdString();
	Float2Attrib *b = new Float2Attrib(snm);
	float tmp[2];
	QJsonArray &fv = content["value"].toArray();
	tmp[0] = fv[0].toDouble();
	tmp[1] = fv[1].toDouble();
	b->setValue(tmp);
	QJsonArray &fm = content["min"].toArray();
	tmp[0] = fm[0].toDouble();
	tmp[1] = fm[1].toDouble();
	b->setMin(tmp);
	QJsonArray &fx = content["max"].toArray();
	tmp[0] = fx[0].toDouble();
	tmp[1] = fx[1].toDouble();
	b->setMax(tmp);
	return b;
}

QAttrib *AttribCreator::createFloat3Attribute(const QJsonObject &content)
{
	QString name = content["name"].toString();
	std::string snm = name.toStdString();
	Float3Attrib *b = new Float3Attrib(snm);
	float tmp[3];
	QJsonArray &fv = content["value"].toArray();
	tmp[0] = fv[0].toDouble();
	tmp[1] = fv[1].toDouble();
    tmp[2] = fv[2].toDouble();
	b->setValue(tmp);
	QJsonArray &fm = content["min"].toArray();
	tmp[0] = fm[0].toDouble();
	tmp[1] = fm[1].toDouble();
    tmp[2] = fm[2].toDouble();
	b->setMin(tmp);
	QJsonArray &fx = content["max"].toArray();
	tmp[0] = fx[0].toDouble();
	tmp[1] = fx[1].toDouble();
    tmp[2] = fx[2].toDouble();
	b->setMax(tmp);
	return b;
}

QAttrib *AttribCreator::createMeshAttribute(const QJsonObject &content)
{
	QString name = content["name"].toString();
	std::string snm = name.toStdString();
	MeshAttrib *b = new MeshAttrib(snm);
	return b;
}

QAttrib *AttribCreator::createListAttribute(const QJsonObject &content)
{
	QString name = content["name"].toString();
	std::string snm = name.toStdString();
	ListAttrib *b = new ListAttrib(snm);
	return b;
}

QAttrib *AttribCreator::createStringAttribute(const QJsonObject &content)
{
	QString name = content["name"].toString();
	int sat = content["strtyp"].toInt();
	std::string snm = name.toStdString();
	StringAttrib *b = new StringAttrib(snm);
	b->setStrAttrTyp(sat);
	return b;
}

void AttribCreator::addTransformAttributes(GlyphOps *ops)
{
	bool stat;
	QJsonObject content = getTransformPresetObj(stat);
	if(!stat) return;

	addAttributes(ops, content);
}

QJsonObject AttribCreator::getTransformPresetObj(bool &found)
{
	found = false;
	QJsonArray attrArray = attributePresetObj["children"].toArray();
	QJsonObject res;
	for(int i=0;i<attrArray.size();++i) {
		res = attrArray[i].toObject();
		if(res["name"].toString() == "transform") {
			found = true;
			break;
		}
	}
	
	return res;
}

void AttribCreator::addConnection(QAttrib *b, const QJsonObject &content)
{
	if(!content.contains("port")) return;

	QJsonObject portObj = content["port"].toObject();
	Connectable *c = new Connectable;
	c->_isOutgoing = portObj["outgoing"].toBool();
	c->_side = portObj["side"].toInt();
	c->_px = portObj["px"].toInt();
	c->_py = portObj["py"].toInt();
	b->setConnectable(c);
}

} /// end of alo
