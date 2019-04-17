/*
 *  HSsdfIO.h
 *  aloe
 *
 *  Created by jian zhang on 3/10/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_H_SSDF_IO_H
#define ALO_H_SSDF_IO_H

#include <h5/V1H5IO.h>
#include "HSsdf.h"
#include <boost/format.hpp>

namespace alo {

class HSsdfIO : public ver1::H5IO {

public:
	
	HSsdfIO();
	
	bool findSsdf(std::vector<std::string> & names);
	
/// T is svt
	template<typename T>
	bool saveSsdf(const T& svt, const std::string& name );
	
	template<typename T>
	bool loadSsdf(T& svt, const std::string& name );
	
protected:

private:

};

template<typename T>
bool HSsdfIO::saveSsdf(const T& svt, const std::string& name)
{
	ver1::HBase ga("/asset");
	
	std::string fullName = boost::str(boost::format("/asset/%1%") % name );
	
	HSsdf writer(fullName);
	writer. template save<T>(svt);
	
	writer.close();
	
	ga.close();
	
	return true;
}

template<typename T>
bool HSsdfIO::loadSsdf(T& svt, const std::string& name)
{
	HBase ga("/asset");
	
	HSsdf reader(name);
	reader. template load<T>(svt);
	
	reader.close();
	
	ga.close();
	
	return true;
}

}

#endif
