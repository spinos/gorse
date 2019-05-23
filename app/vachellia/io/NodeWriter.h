/*
 *  NodeWriter.h
 *  vachellia
 *
 *  2019/5/20
 */

#ifndef VCHL_NODE_WRITER_H
#define VCHL_NODE_WRITER_H

#include <qt_graph/AttribPreset.h>
#include <h5_graph/HGraphNodeWriter.h>
#include <QJsonArray>

namespace alo {

class GlyphOps;

namespace vchl {
	
class NodeWriter : public HGraphNodeWriter, public AttribPreset
{
	GlyphOps *m_ops;
	H5GraphIO *m_hio;

public:

	void write(H5GraphIO &hio, GlyphOps *ops, const QJsonObject &content);

private:
	
	void writeAttributes(const QJsonArray &content);
	void writeAttribute(const QJsonObject &content);
	void writeBoolAttribute(const std::string &name);
	void writeIntAttribute(const std::string &name);
	void writeFloatAttribute(const std::string &name);
	void writeFloat2Attribute(const std::string &name);
	void writeFloat3Attribute(const std::string &name);
	void writeListAttribute(const std::string &name);
	void writeStringAttribute(const std::string &name);
	void writeTransformAttributes();

};

} /// end of vchl

} /// end of alo

#endif
