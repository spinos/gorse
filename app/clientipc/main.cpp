#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <string>

int main(int argc, char *argv[])
{
   using namespace boost::interprocess;
      std::cout<<"\n Open already created shared memory object.";
      try {
      windows_shared_memory shm (open_only, "MySharedMemory", read_only);
      //Map the whole shared memory in this process
      mapped_region region(shm, read_only);

      //Check that memory was initialized to 1
      int *mem = static_cast<int*>(region.get_address());
      std::cout<<"\n region size is "<<region.get_size()
      <<"\n first num is "<<*mem;
      
        
      std::cout<<"\n passed";
      } catch (...) {
          std::cout<<"\n Cannot open shared memory object.";
          return 1;
      }
      
   return 0;
}
