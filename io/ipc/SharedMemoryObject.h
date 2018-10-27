/*
 *  SharedMemoryObject.h
 *  aloe
 */

#ifndef ALO_IPC_SHARED_MEMORY_OBJECT_H
#define ALO_IPC_SHARED_MEMORY_OBJECT_H

#include <boost/interprocess/mapped_region.hpp>
#include <map>
#include <string>

namespace boost {
namespace interprocess {
//class shared_memory_object;   
class windows_shared_memory;
}
}

namespace alo {

class SharedMemoryObject {

public:

	SharedMemoryObject();
	~SharedMemoryObject();
    
    boost::interprocess::mapped_region *createRegion(const char *name, int size);
    boost::interprocess::mapped_region readRegion(const char *name);

private:
    
};

}

#endif

