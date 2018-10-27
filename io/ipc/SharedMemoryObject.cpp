/*
 *  SharedMemoryObject.cpp
 *  aloe
 */
 
#include "SharedMemoryObject.h"
#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>

using namespace boost::interprocess;

namespace alo {
 
windows_shared_memory* SharedMemoryObject::Shmem;
 
void SharedMemoryObject::CreateShareMem()
{
     Shmem = new windows_shared_memory  (create_only, "MySharedMemory", read_write, 262144);
}

mapped_region SharedMemoryObject::GetMappedRegion()
{
     mapped_region region(*Shmem, read_write);
     return region;
}

void SharedMemoryObject::DestroySharedMem()
{
     delete Shmem;
}

}

