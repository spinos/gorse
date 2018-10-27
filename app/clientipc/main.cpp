#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <ipc/SharedMemoryObject.h>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <string>

int main(int argc, char *argv[])
{
   using namespace boost::interprocess;

      alo::SharedMemoryObject shm;
      mapped_region region = shm.readRegion("MySharedMemory");
      if(region.get_size() < 1) return 1;

      const char *mem = static_cast<const char*>(region.get_address());
      std::cout<<"\n region size is "<<region.get_size()
      <<"\n str info is "<<mem;
        
      std::cout<<"\n passed";
      
   return 0;
}
