/*
 *  NodeReader.cpp
 *  vachellia
 *
 *  2019/5/23
 */

#include "NodeReader.h"
#include "../ops/allOps.h"
#include <QJsonArray>

namespace alo {

namespace vchl {

NodeReader::NodeReader()
{}

NodeReader::~NodeReader()
{}

void NodeReader::read(H5GraphIO &hio, GlyphOps *ops, const QJsonObject &content)
{
    m_ops = ops;
    m_hio = &hio;
    
    QJsonArray attrArray = content["attribs"].toArray();
    readAttributes(attrArray);
}

void NodeReader::readAttributes(const QJsonArray &content)
{
    for(int i=0; i< content.size();++i) {
        QJsonObject attrObject = content[i].toObject();
        readAttribute(attrObject);
    }

}

void NodeReader::readAttribute(const QJsonObject &content)
{
    QString name = content["name"].toString();
    std::string snm = name.toStdString();

    const int typ = content["typ"].toInt();
    switch(typ) {
        case QAttrib::AtBool :
            readBoolAttribute(snm);
            break;
        case QAttrib::AtInt :
            readIntAttribute(snm);
            break;
        case QAttrib::AtFloat :
            readFloatAttribute(snm);
            break;
        case QAttrib::AtFloat2 :
            readFloat2Attribute(snm);
            break;
        case QAttrib::AtFloat3 :
            readFloat3Attribute(snm);
            break;
        case QAttrib::AtMesh :
            break;
        case QAttrib::AtList :
            readListAttribute(snm);
            break;
        case QAttrib::AtString :
            readStringAttribute(snm);
            break;
        case QAttrib::AtTransformSet :
            readTransformAttributes();
            break;
        default:
            break;
    }
}

void NodeReader::readBoolAttribute(const std::string &name)
{
    bool x;
    m_hio->readNodeBoolAttr(name, x);
    m_ops->setBoolAttrValue(name, x);
}

void NodeReader::readIntAttribute(const std::string &name)
{
    int x;
    m_hio->readNodeIntAttr(name, &x);
    m_ops->setIntAttrValue(name, x);
}

void NodeReader::readFloatAttribute(const std::string &name)
{
    float x;
    m_hio->readNodeFloatAttr(name, &x);
    m_ops->setFloatAttrValue(name, x);
}

void NodeReader::readFloat2Attribute(const std::string &name)
{
}

void NodeReader::readFloat3Attribute(const std::string &name)
{
}

void NodeReader::readListAttribute(const std::string &name)
{
}

void NodeReader::readStringAttribute(const std::string &name)
{
    std::string y;
    m_hio->readNodeStringAttr(name, y);
    m_ops->setStringAttrValue(name, y);
}

void NodeReader::readTransformAttributes()
{
    bool stat;
    QJsonObject content = getTransformPresetObj(stat);
    if(!stat) return;

    QJsonArray attrArray = content["attribs"].toArray();
    readAttributes(attrArray);
}

} /// end of vchl

} /// end of alo
