/*
 *  SharedMemoryObject.cpp
 *  aloe
 */
 
#include "SharedMemoryObject.h"
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/windows_shared_memory.hpp>
#include <iostream>

using namespace boost::interprocess;

namespace alo {

SharedMemoryObject::SharedMemoryObject()
{}

SharedMemoryObject::~SharedMemoryObject()
{}
 
mapped_region *SharedMemoryObject::createRegion(const char *name, int size)
{
	std::cout<<" create shared memory object " << name;
	try {
    	windows_shared_memory shm(create_only, name, read_write, size);
    	return new mapped_region(shm, read_write);
    } catch (...) {
    	std::cout << " named region " << name << " already exists ";
    	windows_shared_memory shm(open_only, name, read_write);
    	return new mapped_region(shm, read_write);
    }
}

mapped_region SharedMemoryObject::readRegion(const char *name)
{
	std::cout<<" open shared memory object " << name;
	try {
		windows_shared_memory shm(open_only, name, read_only);
      	return mapped_region(shm, read_only);
	} catch (...) {
		std::cout << " named region " << name << " doesn't exist ";
		return mapped_region();
	}
}

}
