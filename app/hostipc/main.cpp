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
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sstream>
#include <time.h>
#include <jsn/JMesh.h>

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
  using namespace boost::property_tree;
  using namespace alo;

  time_t timer;
/// number of seconds since epoch
  time(&timer);

  ptree pt;
  pt.put("updated", timer);

  JMesh msh1;
  msh1.setName("Foo");
  msh1.setNvNt(798, 1256);
  msh1.addUvSet("map1");
  ptree pm1 = msh1.createPtree();

  JMesh msh2;
  msh2.setName("Bar");
  msh2.setNvNt(998, 1956);
  msh2.addUvSet("map1");
  msh2.addUvSet("set1");
  ptree pm2 = msh2.createPtree();

  ptree meshChildren;
  meshChildren.push_back(std::make_pair("", pm1));
  meshChildren.push_back(std::make_pair("", pm2));

  pt.add_child("meshes", meshChildren);

  std::stringstream sst;
  json_parser::write_json(sst, pt, true);
  std::cout<<" json stream "<<sst.str();


    std::cout<<"\n region page size"<<mapped_region::get_page_size();

   alo::SharedMemoryObject shm;

    mapped_region *region = shm.createRegion("MySharedMemory", 65536);
      
      //std::string info = "hello world {a:{e,f,g},b,c,d}";
    std::string info = sst.str();
      std::memcpy(region->get_address(), info.c_str(), info.size() );

      boost::thread t( boost::bind(HostProc::Launch, 
                        region) );
      t.join();
     //std::cout<<"\n windows_shared_memory is destroyed when the last attached process dies...";
      
   
   return 0;
}
