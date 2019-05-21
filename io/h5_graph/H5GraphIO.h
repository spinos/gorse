/*
 *  H5GraphIO.h
 *  aloe
 *
 *  /scene/node[]
 *
 *  2019/5/20
 */

#ifndef ALO_H5_GRAPH_IO_H
#define ALO_H5_GRAPH_IO_H

#include <h5/V1H5IO.h>

namespace alo {
    
namespace ver1 {
    
class HBase;

}

class H5GraphIO : public ver1::H5IO {
	
	ver1::HBase *m_head;
	ver1::HBase *m_current;

public:

	H5GraphIO();
	~H5GraphIO();

	void sceneBegin();
	void sceneEnd();
	void nodeBegin(const int &nodeId);
	void nodeEnd();

	void writeNodePosition(float x, float y);
	void writeNodeType(int x);
	void writeNodeDisplayName(const std::string &x);

protected:
	
private:
};

} /// end of alo

#endif
