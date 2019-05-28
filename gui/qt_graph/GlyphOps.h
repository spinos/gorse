/*
 *  GlyphOps.h
 *
 *  base ops id, attribute, scene, update, etc
 *  
 *  2019/5/22
 */

#ifndef GORS_GLYPH_OPS_H
#define GORS_GLYPH_OPS_H

#include "Attrib.h"
#include <map>

namespace alo {

class CameraEvent;
struct AFileDlgProfile;
class Hexahedron;
class GlyphScene;
class GlyphConnection;

class GlyphOps
{
	int m_opsId;
	std::map<std::string, QAttrib * > m_attribs;
	GlyphScene *m_gscene;

public:
	GlyphOps();
	virtual ~GlyphOps();

	void addAttribute(QAttrib *b);
	std::map<std::string, QAttrib * >::iterator attribBegin();
	std::map<std::string, QAttrib * >::iterator attribEnd();
	QAttrib *findAttrib(const std::string &attrName);
	bool setIntAttrValue(const std::string &attrName, const int &x);
    bool setFloatAttrValue(const std::string &attrName, const float &x);
    bool setBoolAttrValue(const std::string &attrName, const bool &x);
	bool setListAttrValue(const std::string &attrName, const std::string &itemName);
	bool setStringAttrValue(const std::string &attrName, const std::string &x);

/// group << 10 | object
/// non more than 1023 objects in one group
	void setOpsId(int x);
	const int &opsId() const;
	int opsObjectId() const;
	int opsTypeId() const;

	virtual std::string opsName() const;
	std::string displayName() const;
	
	virtual void update();
    virtual bool hasEnable() const;
    virtual bool hasRenderable() const;
	virtual bool hasDrawable() const;
    
    virtual void setActivated(bool x);
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

	virtual bool canConnectTo(GlyphOps *another, const std::string &portName) const;
	virtual void preConnectTo(GlyphOps *another, const std::string &portName);
	virtual void connectTo(GlyphOps *another, const std::string &portName, GlyphConnection *line);
	virtual void disconnectFrom(GlyphOps *another, const std::string &portName, GlyphConnection *line);
	virtual void postConnectionChange(const std::string &portName);
	virtual void receiveImpulse(GlyphOps *another, const std::string &portName);
	virtual void preDestruction();
	virtual QString getShortDescription() const;

	void setGlyphScene(GlyphScene *x);
	GlyphScene *glyphScene();

	bool getIntAttribValue(int &val, const std::string &attrName);
	bool getFloatAttribValue(float &val, const std::string &attrName);
	bool getFloat2AttribValue(float *val, const std::string &attrName);
	bool getFloat3AttribValue(float *val, const std::string &attrName);
	bool getBoolAttribValue(bool &val, const std::string &attrName);
	bool getStringAttribValue(std::string &val, const std::string &attrName);
	bool getListAttribValue(std::string &val, const std::string &attrName);

	virtual bool getVisibleState() const;
	virtual bool getActivatedState() const;

	virtual void preSave();
	virtual void postLoad();

protected:

private:
	
	void setFloatComponentAttrValue(QAttrib *attr, const int &component, const float &x);
	
};

}

#endif
