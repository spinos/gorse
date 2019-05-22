/*
 *  NodeWriter.h
 *  vachellia
 *
 *  2019/5/20
 */

#ifndef VCHL_NODE_WRITER_H
#define VCHL_NODE_WRITER_H

#include <h5_graph/HGraphNodeWriter.h>
#include <QJsonObject>

namespace alo {

class GlyphOps;

namespace vchl {
	
class NodeWriter : public HGraphNodeWriter {

public:

	void write(H5GraphIO &hio, GlyphOps *ops, const QJsonObject &content);

private:
	
	void writeAttribute(H5GraphIO &hio, GlyphOps *ops, const QJsonObject &content);
	void writeBoolAttribute(H5GraphIO &hio, GlyphOps *ops, const std::string &name);
	void writeIntAttribute(H5GraphIO &hio, GlyphOps *ops, const std::string &name);
	void writeFloatAttribute(H5GraphIO &hio, GlyphOps *ops, const std::string &name);
	void writeFloat2Attribute(H5GraphIO &hio, GlyphOps *ops, const std::string &name);
	void writeFloat3Attribute(H5GraphIO &hio, GlyphOps *ops, const std::string &name);
	void writeMeshAttribute(H5GraphIO &hio, GlyphOps *ops, const std::string &name);
	void writeListAttribute(H5GraphIO &hio, GlyphOps *ops, const std::string &name);
	void writeStringAttribute(H5GraphIO &hio, GlyphOps *ops, const std::string &name);

};

} /// end of vchl

} /// end of alo

#endif
