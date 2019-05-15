#include "GlyphOps.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QDebug>

namespace alo {

QJsonObject GlyphOps::attributePresetObj;

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

std::string GlyphOps::opsName() const
{ return "ops"; }

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
		case QAttrib::AtFloat2 :
			b = addFloat2Attribute(content);
			break;
        case QAttrib::AtFloat3 :
			b = addFloat3Attribute(content);
			break;
		case QAttrib::AtMesh :
			b = addMeshAttribute(content);
			break;
		case QAttrib::AtList :
			b = addListAttribute(content);
			break;
		case QAttrib::AtString :
			b = addStringAttribute(content);
			break;
		case QAttrib::AtTransformSet :
			addTransformAttributes();
			break;
		default:
			break;
	}

	if(b) {
		b->setLabel(content["label"].toString().toStdString());
		addConnection(b, content);
	}
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

QAttrib *GlyphOps::addFloat2Attribute(const QJsonObject &content)
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
	m_attribs[snm] = b;
	return b;
}

QAttrib *GlyphOps::addFloat3Attribute(const QJsonObject &content)
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
	if(!content.contains("port")) return;

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
	
	update();
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

	update();
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
	
	update();
	return true;
}

QAttrib *GlyphOps::addListAttribute(const QJsonObject &content)
{
	QString name = content["name"].toString();
	std::string snm = name.toStdString();
	ListAttrib *b = new ListAttrib(snm);
	m_attribs[snm] = b;
	return b;
}

QAttrib *GlyphOps::addStringAttribute(const QJsonObject &content)
{
	QString name = content["name"].toString();
	int sat = content["strtyp"].toInt();
	std::string snm = name.toStdString();
	StringAttrib *b = new StringAttrib(snm);
	b->setStrAttrTyp(sat);
	m_attribs[snm] = b;
	return b;
}

bool GlyphOps::setListAttrValue(const std::string &attrName, const std::string &itemName)
{
	QAttrib *attr = findAttrib(attrName);
	if(!attr) return false;
	
	ListAttrib *fattr = static_cast<ListAttrib *>(attr);
	fattr->setValue(itemName);
	
	update();
    return true; 
}

bool GlyphOps::setStringAttrValue(const std::string &attrName, const std::string &x)
{
	QAttrib *attr = findAttrib(attrName);
	if(!attr) return false;
	
	StringAttrib *fattr = static_cast<StringAttrib *>(attr);
	fattr->setValue(x);
	
	update();
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

bool GlyphOps::getListAttribValue(std::string &val, const std::string &attrName)
{
	QAttrib * attr = findAttrib(attrName);
	if(!attr) return false;

    ListAttrib *fattr = static_cast<ListAttrib *>(attr);
    fattr->getValue(val);
    return true;
}

void GlyphOps::addTransformAttributes()
{
	bool stat;
	QJsonObject content = getTransformPresetObj(stat);
	if(!stat) return;

	addAttributes(content);
}

void GlyphOps::loadAttributePreset(const QString &fileName)
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

QJsonObject GlyphOps::getTransformPresetObj(bool &found)
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
	std::cout << "\n GlyphOps::preDestruction "; 
}

void GlyphOps::receiveImpulse(GlyphOps *another, const std::string &portName)
{
	std::cout << "\n GlyphOps::receiveImpulse from " << another << " via " << portName;
}

} /// end of namespace alo
