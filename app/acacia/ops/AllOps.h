#ifndef ACA_ALL_OPS_H
#define ACA_ALL_OPS_H

#include "RootOps.h"
#include "LodMeshOps.h"
#include "SimpleMeshOps.h"
#include "EdgeCollapseTest.h"
#include "LodMeshInOps.h"
#include "PVSTest.h"
#include "SphereOps.h"
#include "GeodesicDistanceTest.h"

namespace alo {

enum AcaciaOpsType {
    AoUnknown = 0,
    AoSphere = 214496,
    AoLodMesh = 277635,
    AoStaticMesh = 299105,
    AoRoot = 872960,
    AoEdgeCollapseTest = 828635,
    AoLodMeshIn,
    AoGeodesicDistanceTest = 704004,
    AoPVSTest = 716941
};

}

#endif

