/*
 *  NodeReader.h
 *  vachellia
 *
 *  2019/5/23
 */

#ifndef VCHL_NODE_READER_H
#define VCHL_NODE_READER_H

#include <qt_graph/AttribPreset.h>
#include <h5_graph/HGraphNodeReader.h>
#include <QJsonArray>

namespace alo {

class GlyphOps;

namespace vchl {
	
class NodeReader : public HGraphNodeReader, public AttribPreset {

    GlyphOps *m_ops;
	H5GraphIO *m_hio;
    
public:

	NodeReader();
	virtual ~NodeReader();
    
    void read(H5GraphIO &hio, GlyphOps *ops, const QJsonObject &content);

private:
	
	void readAttributes(const QJsonArray &content);
    void readAttribute(const QJsonObject &content);
    void readBoolAttribute(const std::string &name);
	void readIntAttribute(const std::string &name);
	void readFloatAttribute(const std::string &name);
	void readFloat2Attribute(const std::string &name);
	void readFloat3Attribute(const std::string &name);
	void readListAttribute(const std::string &name);
	void readStringAttribute(const std::string &name);
    void readTransformAttributes();
    
};

} /// end of vchl

} /// end of alo

#endif
