/*
 *  V1H5IO.cpp
 *  aloe
 *
 *
 */

#include "V1H5IO.h"
#include "V1HBase.h"
#include "PathList.h"

namespace alo {
    
namespace ver1 {

std::string H5IO::BeheadName("");

bool H5IO::begin(const std::string & filename,
					HDocument::OpenMode om)
{
	if(!HObject::FileIO.open(filename.c_str(), om) ) {
		if(om == HDocument::oCreate) 
			std::cout<<"\n  h5io cannot create file "<<filename;
		else if(om == HDocument::oReadAndWrite) 
			std::cout<<"\n  h5io cannot read/write file "<<filename;
		else 
			std::cout<<"\n  h5io cannot read file "<<filename;
		return false;
	}
	m_doc = HObject::FileIO;
	//std::cout<<"\n h5io open file "<<m_doc.fileName()<<"\n";
	return true;
}

void H5IO::end()
{
	//std::cout<<"\n h5io close file "<<m_doc.fileName()<<"\n";
	m_doc.close();
}

bool H5IO::ObjectExists(const std::string & fullPath)
{
    const PathList pl(fullPath);
	bool stat = true;
	HBase rt("/");
	std::vector<HBase *> openedGrps;
    openedGrps.push_back(&rt);

    const int n = pl.length();
    for(int i=0;i<n;++i) {
		stat = openedGrps.back()->hasNamedChild(pl.elementName(i).c_str() );
		
		if(stat ) {
			openedGrps.push_back(new HBase(pl.elementPath(i) ) );
			
		} else {
			std::cout<<"\n  h5io cannot find "<<pl.elementPath(i)
					<<" in file "<<HObject::FileIO.fileName();
			std::cout.flush();
			stat = false;
			break;
		}
    }
	
	std::vector<HBase *>::iterator cit = openedGrps.begin();
    for(;cit!=openedGrps.end();++cit) {
		(*cit)->close();
	}
	
	return stat;
}

void H5IO::CreateGroup(const std::string & name)
{
	const PathList pl(name);
	const int n = pl.length();
    for(int i=0;i<n;++i) {
        HBase grpPar(pl.elementPath(i) );
        grpPar.close();
    }
}

/// behead and strip ns
void H5IO::H5PathName(std::string & dst)
{
	PathList pl(dst);
	pl.behead(BeheadName);
	dst = pl.lastPathStripNs();
}

HBase * H5IO::GetH5dHeadGroup()
{
    if(BeheadName.size() < 2) {
        return new HBase("/");
    }
    
    if(!ObjectExists(BeheadName)) {
        return new HBase("/");
   }
   
   std::cout<<"\n h5 head name is "<<BeheadName;
   return new HBase(BeheadName);
}

}

}
