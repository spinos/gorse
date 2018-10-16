#include "GlyphOps.h"
#include <QJsonArray>
#include <QDebug>

namespace alo {

GlyphOps::GlyphOps()
{}

GlyphOps::~GlyphOps()
{
	std::map<std::string, QAttrib * >::iterator it = m_attribs.begin();
	for(;it!=m_attribs.end();++it)
		delete it->second;
	m_attribs.clear();
}

void GlyphOps::update() 
{}

void GlyphOps::addAttributes(const QJsonObject &content)
{
	QJsonArray attrArray = content["attribs"].toArray();
    for(int i=0; i< attrArray.size();++i) {
    	QJsonObject attrObject = attrArray[i].toObject();
		addAttribute(attrObject);
	}
}

void GlyphOps::addAttribute(const QJsonObject &content)
{
	int typ = content["typ"].toInt();
	QAttrib *b = 0;
	switch(typ) {
		case QAttrib::AtBool :
			b = addBoolAttribute(content);
			break;
		case QAttrib::AtInt :
			b = addIntAttribute(content);
			break;
		case QAttrib::AtFloat :
			b = addFloatAttribute(content);
			break;
		case QAttrib::AtMesh :
			b = addMeshAttribute(content);
			break;
		default:
			break;
	}

	if(b)
		addConnection(b, content);
}

QAttrib *GlyphOps::addBoolAttribute(const QJsonObject &content)
{
	QString name = content["name"].toString();
	std::string snm = name.toStdString();
	BoolAttrib *b = new BoolAttrib(snm);
	b->setValue(content["value"].toBool());
	m_attribs[snm] = b;
	return b;
}

QAttrib *GlyphOps::addIntAttribute(const QJsonObject &content)
{
	QString name = content["name"].toString();
	std::string snm = name.toStdString();
	IntAttrib *b = new IntAttrib(snm);
	b->setValue(content["value"].toInt());
	b->setMin(content["min"].toInt());
	b->setMax(content["max"].toInt());
	m_attribs[snm] = b;
	return b;
}

QAttrib *GlyphOps::addFloatAttribute(const QJsonObject &content)
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
	m_attribs[snm] = b;
	return b;
}

QAttrib *GlyphOps::addMeshAttribute(const QJsonObject &content)
{
	QString name = content["name"].toString();
	std::string snm = name.toStdString();
	MeshAttrib *b = new MeshAttrib(snm);
	m_attribs[snm] = b;
	return b;
}

void GlyphOps::addConnection(QAttrib *b, const QJsonObject &content)
{
	if(!content.contains("port"))
		return;

	QJsonObject portObj = content["port"].toObject();
	Connectable *c = new Connectable;
	c->_isOutgoing = portObj["outgoing"].toBool();
	c->_side = portObj["side"].toInt();
	c->_px = portObj["px"].toInt();
	c->_py = portObj["py"].toInt();
	b->setConnectable(c);
}

std::map<std::string, QAttrib * >::iterator GlyphOps::attribBegin()
{ return m_attribs.begin(); }

std::map<std::string, QAttrib * >::iterator GlyphOps::attribEnd()
{ return m_attribs.end(); }

}