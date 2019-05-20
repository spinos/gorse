/*
 *  NodeWriter.h
 *  vachellia
 *
 *  2019/5/20
 */

#ifndef VCHL_NODE_WRITER_H
#define VCHL_NODE_WRITER_H

#include <h5_graph/H5GraphIO.h>

namespace alo {

class GlyphOps;

namespace vchl {
	
class NodeWriter  {

public:

	void write(H5GraphIO &hio, const GlyphOps *ops);

};

} /// end of vchl

} /// end of alo

#endif
