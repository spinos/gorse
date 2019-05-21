/*
 *  HGraphNodeWriter.cpp
 *  vachellia
 *
 *  2019/5/20
 */

#include "HGraphNodeWriter.h"
#include <qt_graph/GlyphOps.h>

namespace alo {

void HGraphNodeWriter::writeNodeOps(H5GraphIO &hio, const GlyphOps *ops)
{
	const int typeId = ops->opsTypeId();
	std::cout << "\n write ops "<<typeId;
	hio.writeNodeType(typeId);
	hio.writeNodeDisplayName(ops->displayName());
}

} /// end of alo
