/*
 *  NodeWriter.cpp
 *  vachellia
 *
 *  2019/5/20
 */

#include "NodeWriter.h"
#include "../ops/allOps.h"

namespace alo {

namespace vchl {

void NodeWriter::write(H5GraphIO &hio, GlyphOps *ops, const QJsonObject &content)
{
    writeNodeOps(hio, ops);

    m_ops = ops;
    m_hio = &hio;

    QJsonArray attrArray = content["attribs"].toArray();
    writeAttributes(attrArray);
}

void NodeWriter::writeAttributes(const QJsonArray &content)
{
    for(int i=0; i< content.size();++i) {
        QJsonObject attrObject = content[i].toObject();
        writeAttribute(attrObject);
    }

}

void NodeWriter::writeAttribute(const QJsonObject &content)
{
    QString name = content["name"].toString();
    std::string snm = name.toStdString();

    const int typ = content["typ"].toInt();
    switch(typ) {
        case QAttrib::AtBool :
            writeBoolAttribute(snm);
            break;
        case QAttrib::AtInt :
            writeIntAttribute(snm);
            break;
        case QAttrib::AtFloat :
            writeFloatAttribute(snm);
            break;
        case QAttrib::AtFloat2 :
            writeFloat2Attribute(snm);
            break;
        case QAttrib::AtFloat3 :
            writeFloat3Attribute(snm);
            break;
        case QAttrib::AtMesh :
            break;
        case QAttrib::AtList :
            writeListAttribute(snm);
            break;
        case QAttrib::AtString :
            writeStringAttribute(snm);
            break;
        case QAttrib::AtTransformSet :
            writeTransformAttributes();
            break;
        default:
            break;
    }
}

void NodeWriter::writeBoolAttribute(const std::string &name)
{
    bool x;
    m_ops->getBoolAttribValue(x, name);
    m_hio->writeNodeBoolAttr(name, x);
}

void NodeWriter::writeIntAttribute(const std::string &name)
{
    int x;
    m_ops->getIntAttribValue(x, name);
    m_hio->writeNodeIntAttr(name, 1, &x);
}

void NodeWriter::writeFloatAttribute(const std::string &name)
{
    float x;
    m_ops->getFloatAttribValue(x, name);
    m_hio->writeNodeFloatAttr(name, 1, &x);
}

void NodeWriter::writeFloat2Attribute(const std::string &name)
{
    float x[2];
    m_ops->getFloat2AttribValue(x, name);
    m_hio->writeNodeFloatAttr(name, 2, x);
}

void NodeWriter::writeFloat3Attribute(const std::string &name)
{
    float x[3];
    m_ops->getFloat3AttribValue(x, name);
    m_hio->writeNodeFloatAttr(name, 3, x);
}

void NodeWriter::writeListAttribute(const std::string &name)
{
    std::string x;
    m_ops->getListAttribValue(x, name);
    m_hio->writeNodeStringAttr(name, x);
}

void NodeWriter::writeStringAttribute(const std::string &name)
{
    std::string x;
    m_ops->getStringAttribValue(x, name);
    m_hio->writeNodeStringAttr(name, x);
}

void NodeWriter::writeTransformAttributes()
{
    bool stat;
    QJsonObject content = getTransformPresetObj(stat);
    if(!stat) return;

    QJsonArray attrArray = content["attribs"].toArray();
    writeAttributes(attrArray);
}

} /// end of vchl

} /// end of alo
