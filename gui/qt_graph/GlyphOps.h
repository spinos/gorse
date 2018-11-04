/*
 *  GlyphOps.h
 *
 *  attribute and update
 *
 */

#ifndef GAR_GLYPH_OPS_H
#define GAR_GLYPH_OPS_H

#include "Attrib.h"
#include <map>
#include <QJsonObject>

namespace alo {

class GlyphOps
{
	std::map<std::string, QAttrib * > m_attribs;

public:
	GlyphOps();
	virtual ~GlyphOps();

	void addAttributes(const QJsonObject &content);
	std::map<std::string, QAttrib * >::iterator attribBegin();
	std::map<std::string, QAttrib * >::iterator attribEnd();
	QAttrib *findAttrib(const std::string &attrName);
	bool setFloatAttrValue(const std::string &attrName, const float &x);
    bool setListAttrValue(const std::string &attrName, const std::string &itemName);
	
	virtual void update();
	virtual bool hasDrawable() const;
	virtual void onSelection();
	virtual void postUI();

protected:
	void addAttribute(const QJsonObject &content);
	QAttrib *addBoolAttribute(const QJsonObject &content);
	QAttrib *addIntAttribute(const QJsonObject &content);
	QAttrib *addFloatAttribute(const QJsonObject &content);
	QAttrib *addFloat2Attribute(const QJsonObject &content);
	QAttrib *addMeshAttribute(const QJsonObject &content);
	QAttrib *addListAttribute(const QJsonObject &content);

private:
	void addConnection(QAttrib *b, const QJsonObject &content);
	void setFloatComponentAttrValue(QAttrib *attr, const int &component, const float &x);

};

}

#endif
