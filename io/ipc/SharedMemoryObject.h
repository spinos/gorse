/*
 *  SharedMemoryObject.h
 *  aloe
 */

#ifndef KLP_IPC_SHARED_MEMORY_OBJECT_H
#define KLP_IPC_SHARED_MEMORY_OBJECT_H

namespace boost {
namespace interprocess {
class windows_shared_memory;   
class mapped_region;
}
}

namespace alo {

class SharedMemoryObject {
    
public:
    
    static void CreateShareMem();
    static boost::interprocess::mapped_region GetMappedRegion();
    static void DestroySharedMem();
    
private:
    
    static boost::interprocess::windows_shared_memory* Shmem;
    
};

}

#endif

