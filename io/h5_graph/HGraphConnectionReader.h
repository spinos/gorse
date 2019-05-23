/*
 *  HGraphConnectionReader.h
 *  gorse
 *
 *  base connection reader 
 *
 *  2019/5/25
 */

#ifndef ALO_GRAPH_CONNECTION_READER_H
#define ALO_GRAPH_CONNECTION_READER_H

#include "H5GraphIO.h"

namespace alo {
	
class HGraphConnectionReader {

	int m_connectionUid;
	int m_nodeIds[2];
	std::string m_port0Name;
	std::string m_port1Name;

public:

	HGraphConnectionReader();
	virtual ~HGraphConnectionReader();

	void readConnection(H5GraphIO &hio);

	const int &connectionId() const;
	const int &node0Id() const;
	const int &node1Id() const;
	const std::string &port0Name() const;
	const std::string &port1Name() const;
	
protected:

};

} /// end of alo

#endif
