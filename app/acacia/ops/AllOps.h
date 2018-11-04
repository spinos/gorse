#ifndef ACA_ALL_OPS_H
#define ACA_ALL_OPS_H

#include "RootOps.h"
#include "MeshListenerOps.h"
#include "EdgeCollapseTest.h"

namespace alo {

enum AcaciaOpsType {
    AoUnknown = 0,
    AoMeshListener = 277635,
    AoRoot = 872960,
    AoEdgeCollapseTest = 828635
};

}

#endif

