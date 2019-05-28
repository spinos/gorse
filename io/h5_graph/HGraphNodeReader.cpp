/*
 *  HGraphNodeReader.cpp
 *  vachellia
 *
 *  2019/5/26
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
    hio.readNodeVisibleState(m_isVisible);
    hio.readNodeActivatedState(m_isActivated);
}

const int &HGraphNodeReader::nodeUid() const
{ return m_nodeUid; }

int HGraphNodeReader::nodeTypeId() const
{ return m_nodeUid >> 10; }

const std::string &HGraphNodeReader::nodeDisplayName() const
{ return m_dspName; }

const float *HGraphNodeReader::nodePosition() const
{ return m_pos; }

const bool &HGraphNodeReader::isVisible() const
{ return m_isVisible; }

const bool &HGraphNodeReader::isActivated() const
{ return m_isActivated; }

} /// end of alo
