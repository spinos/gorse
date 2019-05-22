/*
 *  NodeWriter.cpp
 *  vachellia
 *
 *  2019/5/20
 */

#include "NodeWriter.h"
#include "../ops/allOps.h"
#include <QJsonArray>

namespace alo {

namespace vchl {

void NodeWriter::write(H5GraphIO &hio, GlyphOps *ops, const QJsonObject &content)
{
    writeNodeOps(hio, ops);

    QJsonArray attrArray = content["attribs"].toArray();
    for(int i=0; i< attrArray.size();++i) {
        QJsonObject attrObject = attrArray[i].toObject();
        writeAttribute(hio, ops, attrObject);
    }

}

void NodeWriter::writeAttribute(H5GraphIO &hio, GlyphOps *ops, const QJsonObject &content)
{
    QString name = content["name"].toString();
    std::string snm = name.toStdString();

    const int typ = content["typ"].toInt();
    switch(typ) {
        case QAttrib::AtBool :
            writeBoolAttribute(hio, ops, snm);
            break;
        case QAttrib::AtInt :
            writeIntAttribute(hio, ops, snm);
            break;
        case QAttrib::AtFloat :
            writeFloatAttribute(hio, ops, snm);
            break;
        case QAttrib::AtFloat2 :
            writeFloat2Attribute(hio, ops, snm);
            break;
        case QAttrib::AtFloat3 :
            writeFloat3Attribute(hio, ops, snm);
            break;
        case QAttrib::AtMesh :
            //writeMeshAttribute(hio, ops, snm);
            break;
        case QAttrib::AtList :
            writeListAttribute(hio, ops, snm);
            break;
        case QAttrib::AtString :
            writeStringAttribute(hio, ops, snm);
            break;
        case QAttrib::AtTransformSet :
            //writeTransformAttributes(ops);
            break;
        default:
            break;
    }
}

void NodeWriter::writeBoolAttribute(H5GraphIO &hio, GlyphOps *ops, const std::string &name)
{
    bool x;
    ops->getBoolAttribValue(x, name);
    hio.writeNodeBoolAttr(name, x);
}

void NodeWriter::writeIntAttribute(H5GraphIO &hio, GlyphOps *ops, const std::string &name)
{
    int x;
    ops->getIntAttribValue(x, name);
    hio.writeNodeIntAttr(name, 1, &x);
}

void NodeWriter::writeFloatAttribute(H5GraphIO &hio, GlyphOps *ops, const std::string &name)
{
    float x;
    ops->getFloatAttribValue(x, name);
    hio.writeNodeFloatAttr(name, 1, &x);
}

void NodeWriter::writeFloat2Attribute(H5GraphIO &hio, GlyphOps *ops, const std::string &name)
{
    float x[2];
    ops->getFloat2AttribValue(x, name);
    hio.writeNodeFloatAttr(name, 2, x);
}

void NodeWriter::writeFloat3Attribute(H5GraphIO &hio, GlyphOps *ops, const std::string &name)
{
    float x[3];
    ops->getFloat3AttribValue(x, name);
    hio.writeNodeFloatAttr(name, 3, x);
}

void NodeWriter::writeMeshAttribute(H5GraphIO &hio, GlyphOps *ops, const std::string &name)
{
}

void NodeWriter::writeListAttribute(H5GraphIO &hio, GlyphOps *ops, const std::string &name)
{
    std::string x;
    ops->getListAttribValue(x, name);
    hio.writeNodeStringAttr(name, x);
}

void NodeWriter::writeStringAttribute(H5GraphIO &hio, GlyphOps *ops, const std::string &name)
{
    std::string x;
    ops->getStringAttribValue(x, name);
    hio.writeNodeStringAttr(name, x);
}


} /// end of vchl

} /// end of alo
