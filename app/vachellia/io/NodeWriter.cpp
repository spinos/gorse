/*
 *  NodeWriter.cpp
 *  vachellia
 *
 *  2019/5/20
 */

#include "NodeWriter.h"
#include "../ops/allOps.h"
#include <qt_graph/GlyphOps.h>
#include <h5_graph/HGraphNodeWriter.h>

namespace alo {

namespace vchl {

void NodeWriter::write(H5GraphIO &hio, const GlyphOps *ops)
{
	const int typeId = ops->opsTypeId();
	switch(typeId) {
        case ConeOps::Type :
            std::cout << " write ConeOps";
            break;
        case CylinderOps::Type :
            std::cout << " write CylinderOps";
            break;
        case SphereOps::Type :
            std::cout << " write SphereOps";
            break;
        case BoxOps::Type :
            std::cout << " write BoxOps";
            break;
        case HorizonOps::Type :
            std::cout << " write HorizonOps";
            break;
        case VoxelOps::Type :
            std::cout << " write VoxelOps";
            break;
        case RepeatOps::Type :
            std::cout << " write RepeatOps";
            break;
        case UnionOps::Type :
            std::cout << " write UnionOps";
            break;
        default:
            ;
    }
}

} /// end of vchl

} /// end of alo
