/*
 *  HGraphNodeWriter.h
 *  gorse
 *
 *  base node writer 
 *
 *  2019/5/20
 */

#ifndef ALO_GRAPH_NODE_WRITER_H
#define ALO_GRAPH_NODE_WRITER_H

#include "H5GraphIO.h"

namespace alo {

class GlyphOps;
	
class HGraphNodeWriter {

public:

	virtual void writeNodeOps(H5GraphIO &hio, const GlyphOps *ops);
	
protected:

};

} /// end of alo

#endif
