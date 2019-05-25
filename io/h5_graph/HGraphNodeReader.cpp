/*
 *  HGraphNodeReader.cpp
 *  vachellia
 *
 *  2019/5/23
 */

#include "HGraphNodeReader.h"
#include <qt_graph/GlyphOps.h>

namespace alo {

HGraphNodeReader::HGraphNodeReader()
{}

HGraphNodeReader::~HGraphNodeReader()
{}

void HGraphNodeReader::readNode(H5GraphIO &hio)
{
	hio.readNodeId(m_nodeUid);
	hio.readNodeDisplayName(m_dspName);
	hio.readNodePosition(m_pos);
	std::cout << "\n read node "<<m_dspName;
}

const int &HGraphNodeReader::nodeUid() const
{ return m_nodeUid; }

int HGraphNodeReader::nodeTypeId() const
{ return m_nodeUid >> 10; }

const std::string &HGraphNodeReader::nodeDisplayName() const
{ return m_dspName; }

const float *HGraphNodeReader::nodePosition() const
{ return m_pos; }

} /// end of alo
