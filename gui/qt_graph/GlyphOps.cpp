/*
 *  GlyphOps.cpp
 *
 *  2019/5/22
 */
 
#include "GlyphOps.h"
#include <QDebug>
#include <boost/format.hpp>

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

void GlyphOps::setOpsId(int x)
{ m_opsId = x; }

const int &GlyphOps::opsId() const
{ return m_opsId; }

int GlyphOps::opsObjectId() const
{ return m_opsId & 1023; }

int GlyphOps::opsTypeId() const
{ return m_opsId >> 10; }

std::string GlyphOps::opsName() const
{ return "ops"; }

std::string GlyphOps::displayName() const
{ return boost::str(boost::format("%1%_%2%") % opsName() % opsObjectId() ); }

void GlyphOps::update()
{ qDebug()<<"GlyphOps::update"; }

bool GlyphOps::hasEnable() const
{ return false; }

bool GlyphOps::hasRenderable() const
{ return false; }

bool GlyphOps::hasDrawable() const
{ return false; }

void GlyphOps::setActivated(bool x)
{}

void GlyphOps::getDrawableBound(Hexahedron &b) const 
{}

void GlyphOps::setDrawableVisible(bool x)
{}

void GlyphOps::addAttribute(QAttrib *b)
{ m_attribs[b->attrName()] = b; }

std::map<std::string, QAttrib * >::iterator GlyphOps::attribBegin()
{ return m_attribs.begin(); }

std::map<std::string, QAttrib * >::iterator GlyphOps::attribEnd()
{ return m_attribs.end(); }

QAttrib *GlyphOps::findAttrib(const std::string &attrName)
{
	std::map<std::string, QAttrib * >::iterator it = m_attribs.find(attrName);
	if(it == m_attribs.end()) {
		qDebug()<<"GlyphOps::setFloatAttrValue cannot find named attrib "<<attrName.c_str();
		return 0;
	}
	return it->second;
}

bool GlyphOps::setIntAttrValue(const std::string &attrName, const int &x)
{
	QAttrib *attr = findAttrib(attrName);
	if(!attr) return false;
	
	IntAttrib *fattr = static_cast<IntAttrib *>(attr);
	fattr->setValue(x);
	
	return true;
}

bool GlyphOps::setFloatAttrValue(const std::string &attrName, const float &x)
{
	std::string realAttrName = attrName;
	int attrComponent;
	QAttrib::SeparateAttrComponent(realAttrName, attrComponent);

	QAttrib *attr = findAttrib(realAttrName);
	if(!attr) return false;

	if(attrComponent < 0) {
		FloatAttrib *fattr = static_cast<FloatAttrib *>(attr);
		fattr->setValue(x);
		
	} else
		setFloatComponentAttrValue(attr, attrComponent, x);

	return true;
}

void GlyphOps::setFloatComponentAttrValue(QAttrib *attr, const int &component, const float &x)
{
	if(attr->attrType() == QAttrib::AtFloat2) {
		Float2Attrib *fattr = static_cast<Float2Attrib *>(attr);
		fattr->setValue(x, component);
	}
    if(attr->attrType() == QAttrib::AtFloat3) {
		Float3Attrib *fattr = static_cast<Float3Attrib *>(attr);
		fattr->setValue(x, component);
	}
}

bool GlyphOps::setBoolAttrValue(const std::string &attrName, const bool &x)
{
    QAttrib *attr = findAttrib(attrName);
	if(!attr) return false;
	
	BoolAttrib *fattr = static_cast<BoolAttrib *>(attr);
	fattr->setValue(x);
	
	return true;
}

bool GlyphOps::setListAttrValue(const std::string &attrName, const std::string &itemName)
{
	QAttrib *attr = findAttrib(attrName);
	if(!attr) return false;
	
	ListAttrib *fattr = static_cast<ListAttrib *>(attr);
	fattr->setValue(itemName);
	
    return true; 
}

bool GlyphOps::setStringAttrValue(const std::string &attrName, const std::string &x)
{
	QAttrib *attr = findAttrib(attrName);
	if(!attr) return false;
	
	StringAttrib *fattr = static_cast<StringAttrib *>(attr);
	fattr->setValue(x);
	
    return true; 
}

void GlyphOps::onSelection() {}

void GlyphOps::postUI() {}

bool GlyphOps::hasMenu() const
{ return false; }

void GlyphOps::getMenuItems(std::vector<std::pair<std::string, int > > &ks) const {}

void GlyphOps::recvAction(int x) {}

void GlyphOps::recvCameraChanged(const CameraEvent &x) {}

AFileDlgProfile *GlyphOps::writeFileProfileR () const
{ return 0; }

AFileDlgProfile *GlyphOps::readFileProfileR () const
{ return 0; }

bool GlyphOps::getIntAttribValue(int &val, const std::string &attrName)
{
	QAttrib * attr = findAttrib(attrName);
	if(!attr) return false;

    IntAttrib *fattr = static_cast<IntAttrib *>(attr);
    fattr->getValue(val);
    return true;
}

bool GlyphOps::getFloatAttribValue(float &val, const std::string &attrName)
{
	QAttrib * attr = findAttrib(attrName);
	if(!attr) return false;

    FloatAttrib *fattr = static_cast<FloatAttrib *>(attr);
    fattr->getValue(val);
    return true;
}

bool GlyphOps::getFloat2AttribValue(float *val, const std::string &attrName)
{
	QAttrib * attr = findAttrib(attrName);
	if(!attr) return false;

    Float2Attrib *fattr = static_cast<Float2Attrib *>(attr);
    fattr->getValue(val);
    return true;
}

bool GlyphOps::getFloat3AttribValue(float *val, const std::string &attrName)
{
	QAttrib * attr = findAttrib(attrName);
	if(!attr) return false;

    Float3Attrib *fattr = static_cast<Float3Attrib *>(attr);
    fattr->getValue(val);
    return true;
}

bool GlyphOps::getBoolAttribValue(bool &val, const std::string &attrName)
{
	QAttrib * attr = findAttrib(attrName);
	if(!attr) return false;
	
    BoolAttrib *fattr = static_cast<BoolAttrib *>(attr);
    fattr->getValue(val);
    return true;
}

bool GlyphOps::getStringAttribValue(std::string &val, const std::string &attrName)
{
	QAttrib * attr = findAttrib(attrName);
	if(!attr) return false;

	StringAttrib *sattr = static_cast<StringAttrib *>(attr);
	sattr->getValue(val);
	
    return true;
}

bool GlyphOps::getListAttribValue(std::string &val, const std::string &attrName)
{
	QAttrib * attr = findAttrib(attrName);
	if(!attr) return false;

    ListAttrib *fattr = static_cast<ListAttrib *>(attr);
    fattr->getValue(val);
    return true;
}

void GlyphOps::setGlyphScene(GlyphScene *x)
{ m_gscene = x; }

GlyphScene *GlyphOps::glyphScene()
{ return m_gscene; }

bool GlyphOps::canConnectTo(GlyphOps *another, const std::string &portName) const
{ return true; }

void GlyphOps::preConnectTo(GlyphOps *another, const std::string &portName)
{
	std::cout << "\n GlyphOps::preConnectTo " << another << " via " << portName;
}

void GlyphOps::connectTo(GlyphOps *another, const std::string &portName, GlyphConnection *line)
{
	std::cout << "\n GlyphOps::connectTo " << another << " via " << portName;
}

void GlyphOps::disconnectFrom(GlyphOps *another, const std::string &portName, GlyphConnection *line)
{
	std::cout << "\n GlyphOps::disconnectFrom " << another << " via " << portName;
}

void GlyphOps::postConnectionChange(const std::string &portName)
{
	std::cout << "\n GlyphOps::postConnectionChange port " << portName; 
}

void GlyphOps::preDestruction()
{
	std::cout << "\n GlyphOps::preDestruction " << opsId(); 
}

void GlyphOps::receiveImpulse(GlyphOps *another, const std::string &portName)
{
	std::cout << "\n GlyphOps::receiveImpulse from " << another << " via " << portName;
}

QString GlyphOps::getShortDescription() const
{ return QString::fromStdString(opsName()); }

void GlyphOps::preSave()
{}

void GlyphOps::postLoad()
{ update(); }

} /// end of namespace alo
