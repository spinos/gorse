/*
 *  HGraphConnectionReader.cpp
 *  vachellia
 *
 *  2019/5/25
 */

#include "HGraphConnectionReader.h"
#include <iostream>

namespace alo {

HGraphConnectionReader::HGraphConnectionReader() : m_connectionUid(-1)
{}

HGraphConnectionReader::~HGraphConnectionReader()
{}

void HGraphConnectionReader::readConnection(H5GraphIO &hio)
{
	hio.readConnectionId(m_connectionUid);
	hio.readConnectionNodeIds(m_nodeIds);
	hio.readConnectionPortNames(m_port0Name, m_port1Name);
}

const int &HGraphConnectionReader::connectionId() const
{ return m_connectionUid; }

const int &HGraphConnectionReader::node0Id() const
{ return m_nodeIds[0]; }

const int &HGraphConnectionReader::node1Id() const
{ return m_nodeIds[1]; }

const std::string &HGraphConnectionReader::port0Name() const
{ return m_port0Name; }

const std::string &HGraphConnectionReader::port1Name() const
{ return m_port1Name; }

} /// end of alo
