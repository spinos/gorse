/*
 *  HGraphConnectionWriter.h
 *  gorse
 *
 *  base connection writer 
 *
 *  2019/5/24
 */

#ifndef ALO_GRAPH_CONNECTION_WRITER_H
#define ALO_GRAPH_CONNECTION_WRITER_H

#include "H5GraphIO.h"

namespace alo {

class GlyphConnection;

namespace sdb {
template <typename KeyType, typename DataType, int Dim1>
class L3DataIterator;
}
	
class HGraphConnectionWriter {

public:

	void writeConnections(H5GraphIO &hio, sdb::L3DataIterator<int, GlyphConnection *, 128> iter);

	virtual void write(H5GraphIO &hio, const GlyphConnection *connection);

protected:

};

} /// end of alo

#endif
