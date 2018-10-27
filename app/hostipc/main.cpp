#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/offset_ptr.hpp>
#include <boost/thread.hpp>
#include <ipc/SharedMemoryObject.h>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <string>

using namespace boost::interprocess;

class HostProc {

public:
    static void Launch(mapped_region* r);
    
};

void HostProc::Launch(mapped_region* r)
{
    std::cout<<"\n waiting for child process";
    for(;;) {
        
    }
}

int main(int argc, char *argv[])
{
    std::cout<<"\n region page size"<<mapped_region::get_page_size();

   alo::SharedMemoryObject shm;

    mapped_region *region = shm.createRegion("MySharedMemory", 4096);
      
      std::string info = "hello world {a:{e,f,g},b,c,d}";
      std::memcpy(region->get_address(), info.c_str(), info.size() );

      boost::thread t( boost::bind(HostProc::Launch, 
                        region) );
      t.join();
     //std::cout<<"\n windows_shared_memory is destroyed when the last attached process dies...";
      
   
   return 0;
}
