/*
 *  NodeReader.h
 *  vachellia
 *
 *  2019/5/23
 */

#ifndef VCHL_NODE_READER_H
#define VCHL_NODE_READER_H

#include <h5_graph/HGraphNodeReader.h>
#include <QJsonObject>

namespace alo {

class GlyphOps;

namespace vchl {
	
class NodeReader : public HGraphNodeReader {

public:

	NodeReader();
	virtual ~NodeReader();

private:
	
	
};

} /// end of vchl

} /// end of alo

#endif
