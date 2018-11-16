/*
 *  H5IO.h
 *  aloe
 *
 *  Created by jian zhang on 10/20/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_H5_IO_H
#define ALO_H5_IO_H

#include "HDocument.h"
#include "HBase.h"

namespace alo {

class H5IO {
	
	HDocument m_doc;
	
public:

	bool begin(const std::string & filename, 
				HDocument::OpenMode om = HDocument::oReadOnly);
	void end();
	
	static bool objectExists(const std::string & fullPath);
	
	static void CreateGroup(const std::string & name);
	template<typename T1, typename T2>
	static void SaveData(const std::string & name, T2 * data)
	{
		T1 grp(name);
		grp.save(data);
		grp.close();
	}
	
	template<typename T1, typename T2>
	static void LoadData(const std::string & name, T2 * data)
	{
		T1 grp(name);
		grp.load(data);
		grp.close();
	}
	
	static std::string BeheadName;
	
/// / or /head
	static HBase * GetH5dHeadGroup();
							  
protected:
	static void H5PathName(std::string & dst);
	
private:
};

}

#endif
