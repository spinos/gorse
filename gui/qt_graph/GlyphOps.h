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

class CameraEvent;
struct AFileDlgProfile;
class Hexahedron;

class GlyphOps
{
	int m_opsId;
	std::map<std::string, QAttrib * > m_attribs;
	static QJsonObject attributePresetObj;

public:
	GlyphOps();
	virtual ~GlyphOps();

	void addAttributes(const QJsonObject &content);
	std::map<std::string, QAttrib * >::iterator attribBegin();
	std::map<std::string, QAttrib * >::iterator attribEnd();
	QAttrib *findAttrib(const std::string &attrName);
	bool setIntAttrValue(const std::string &attrName, const int &x);
    bool setFloatAttrValue(const std::string &attrName, const float &x);
    bool setBoolAttrValue(const std::string &attrName, const bool &x);
	bool setListAttrValue(const std::string &attrName, const std::string &itemName);
	bool setStringAttrValue(const std::string &attrName, const std::string &x);

	void setOpsId(int x);
	const int &opsId() const;
	
	virtual void update();
    virtual bool hasRenderable() const;
	virtual bool hasDrawable() const;
	virtual void setDrawableVisible(bool x);
	virtual void getDrawableBound(Hexahedron &b) const;
	virtual void onSelection();
	virtual void postUI();
	virtual bool hasMenu() const;
	virtual void getMenuItems(std::vector<std::pair<std::string, int > > &ks) const;
	virtual void recvAction(int x);
	virtual void recvCameraChanged(const CameraEvent &x);
	virtual AFileDlgProfile *writeFileProfileR () const;
	virtual AFileDlgProfile *readFileProfileR () const;

	static void loadAttributePreset(const QString &fileName);

protected:
	void addAttribute(const QJsonObject &content);
	QAttrib *addBoolAttribute(const QJsonObject &content);
	QAttrib *addIntAttribute(const QJsonObject &content);
	QAttrib *addFloatAttribute(const QJsonObject &content);
	QAttrib *addFloat2Attribute(const QJsonObject &content);
	QAttrib *addFloat3Attribute(const QJsonObject &content);
	QAttrib *addMeshAttribute(const QJsonObject &content);
	QAttrib *addListAttribute(const QJsonObject &content);
	QAttrib *addStringAttribute(const QJsonObject &content);

	bool getIntAttribValue(int &val, const std::string &attrName);
	bool getFloatAttribValue(float &val, const std::string &attrName);
	bool getFloat2AttribValue(float *val, const std::string &attrName);
	bool getFloat3AttribValue(float *val, const std::string &attrName);
	bool getBoolAttribValue(bool &val, const std::string &attrName);
	bool getListAttribValue(std::string &val, const std::string &attrName);

private:
	void addConnection(QAttrib *b, const QJsonObject &content);
	void setFloatComponentAttrValue(QAttrib *attr, const int &component, const float &x);
	void addTransformAttributes();
	QJsonObject getTransformPresetObj(bool &found);

};

}

#endif
