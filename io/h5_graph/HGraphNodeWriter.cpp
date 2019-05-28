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
	hio.writeNodeDisplayName(ops->displayName());
    
    if(ops->hasRenderable()) {
        bool x = ops->getVisibleState();
        hio.writeNodeVisibleState(x);
    }
    
    if(ops->hasEnable()) {
        bool x = ops->getActivatedState();
        hio.writeNodeActivatedState(x);
    }
}

} /// end of alo
