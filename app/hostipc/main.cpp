#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/offset_ptr.hpp>
#include <boost/thread.hpp>
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

   std::cout<<"\n Create a native windows shared memory object.";
      windows_shared_memory shm (create_only, "MySharedMemory", read_write, 262144);

      //Map the whole shared memory in this process
      mapped_region region(shm, read_write);
      
      //Write first #
      int an = 799;
      std::memcpy(region.get_address(), &an, 4 );

      boost::thread t( boost::bind(HostProc::Launch, 
                        &region) );
      t.join();
     std::cout<<"\n windows_shared_memory is destroyed when the last attached process dies...";
      
   
   return 0;
}
