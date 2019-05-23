/*
 *  HGraphConnectionWriter.cpp
 *  vachellia
 *
 *  2019/5/20
 */

#include "HGraphConnectionWriter.h"
#include <qt_graph/GlyphConnection.h>
#include <qt_graph/GlyphOps.h>
#include <sdb/L3DataIterator.h>

namespace alo {

void HGraphConnectionWriter::writeConnections(H5GraphIO &hio, sdb::L3DataIterator<int, GlyphConnection *, 128> iter)
{
    for(;!iter.done();iter.next()) {
        const int &i = iter.first;
        const GlyphConnection *ci = *iter.second;
        const int &cid = ci->connectionId();
        
        if(cid != i) continue;
        
        hio.connectionBegin(cid);

        write(hio, ci);
    
        hio.connectionEnd();
    }
}

void HGraphConnectionWriter::write(H5GraphIO &hio, const GlyphConnection *connection)
{
	const int &id = connection->connectionId();
	int nd[2];
	connection->getNodeIds(nd);
	std::string pn[2];
	connection->getPortNames(pn[0], pn[1]);
	hio.writeConnectionNodeIds(nd);
	hio.writeConnectionPortNames(pn[0], pn[1]);
}

} /// end of alo
