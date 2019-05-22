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
#include <vector>

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
	void writeNodeId(int x);
	void writeNodeDisplayName(const std::string &x);
	void writeNodeIntAttr(const std::string &name, int dim, const int *x);
	void writeNodeFloatAttr(const std::string &name, int dim, const float *x);
	void writeNodeBoolAttr(const std::string &name, const bool &x);
	void writeNodeStringAttr(const std::string &name, const std::string &x);

	bool openScene();
	void closeScene();

	void lsNodes(std::vector<std::string> &names);
	void openNode(const std::string &name);
	void closeNode();

	void readNodeId(int &y);
	void readNodeDisplayName(std::string &y);
	void readNodePosition(float *y);

protected:
	
private:
};

} /// end of alo

#endif
