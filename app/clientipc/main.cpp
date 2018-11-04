#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <ipc/SharedMemoryObject.h>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sstream>
#include <boost/foreach.hpp>
#include <jsn/JMesh.h>

int main(int argc, char *argv[])
{
   using namespace boost::interprocess;
   using namespace boost::property_tree;
   using namespace alo;

      alo::SharedMemoryObject shm;
      mapped_region region = shm.readRegion("MySharedMemory");
      if(region.get_size() < 1) return 1;

      const char *mem = static_cast<const char*>(region.get_address());
      std::cout<<"\n region size is "<<region.get_size();

      	std::stringstream sst(mem);

      	ptree pt;
      	json_parser::read_json(sst, pt);

      	unsigned ut = pt.get<unsigned>("updated");  
  		std::cout<<" pt.updated: "<<ut;

  		BOOST_FOREACH(ptree::value_type const&v, pt.get_child("meshes")) {

				JMesh msh;
				msh.readPtree(v.second);
				std::cout<<"\n "<<msh;
  		}
        
      std::cout<<"\n passed";
      
   return 0;
}
