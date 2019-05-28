/*
 *  HGraphNodeReader.h
 *  gorse
 *
 *  base node reader 
 *
 *  2019/5/26
 */

#ifndef ALO_GRAPH_NODE_READER_H
#define ALO_GRAPH_NODE_READER_H

#include "H5GraphIO.h"

namespace alo {

class GlyphOps;
	
class HGraphNodeReader {

	std::string m_dspName;
	int m_nodeUid;
	float m_pos[2];
    bool m_isVisible;
    bool m_isActivated;

public:

	HGraphNodeReader();
	virtual ~HGraphNodeReader();

	void readNode(H5GraphIO &hio);

	const int &nodeUid() const;
	int nodeTypeId() const;
	const std::string &nodeDisplayName() const;
	const float *nodePosition() const;
    const bool &isVisible() const;
    const bool &isActivated() const;
	
protected:

};

} /// end of alo

#endif
