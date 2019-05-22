/*
 *  H5GraphIO.cpp
 *  aloe
 *
 *  2019/5/20
 */

#include "H5GraphIO.h"
#include <h5/V1HBase.h>
#include <boost/format.hpp>
#include <ctime>

namespace alo {
    
H5GraphIO::H5GraphIO()
{}

H5GraphIO::~H5GraphIO()
{}

void H5GraphIO::sceneBegin()
{
	m_head = new ver1::HBase("/scene");
	m_head->addIntAttr(".is_graph_scene");
	int one = 1;
	m_head->writeIntAttr(".is_graph_scene", &one);
}

void H5GraphIO::sceneEnd()
{
	std::time_t tnow = std::time(nullptr);
	int t = tnow;
	m_head->addIntAttr(".modified");
	m_head->writeIntAttr(".modified", &t);
	m_head->close();
	delete m_head;
}

void H5GraphIO::nodeBegin(const int &nodeId)
{
	const std::string nodeName = boost::str(boost::format("node_%1%") % nodeId );
	std::cout << "\n node begin " <<m_head->childPath(nodeName);
	m_current = new ver1::HBase(m_head->childPath(nodeName));
	m_current->addIntAttr(".is_graph_node");
	int one = nodeId;
	m_current->writeIntAttr(".is_graph_node", &one);
}

void H5GraphIO::nodeEnd()
{
	std::cout << "\n node end " <<m_current->pathToObject();
	m_current->close();
	delete m_current;
}

void H5GraphIO::writeNodePosition(float x, float y)
{
	float d[2];
	d[0] = x; d[1] = y;
	m_current->addFloatAttr(".pos");
	m_current->writeFloatAttr(".pos", d);
}

void H5GraphIO::writeNodeType(int x)
{
	m_current->addIntAttr(".typ");
	m_current->writeIntAttr(".typ", &x);
}

void H5GraphIO::writeNodeDisplayName(const std::string &x)
{
	m_current->addVLStringAttr(".dspnm");
	m_current->writeVLStringAttr(".dspnm", x);
}

void H5GraphIO::writeNodeIntAttr(const std::string &name, int dim, const int *x)
{
	m_current->addIntAttr(name.c_str(), dim);
	m_current->writeIntAttr(name.c_str(), (int *)x);
}

void H5GraphIO::writeNodeFloatAttr(const std::string &name, int dim, const float *x)
{
	m_current->addFloatAttr(name.c_str(), dim);
	m_current->writeFloatAttr(name.c_str(), (float *)x);
}

void H5GraphIO::writeNodeBoolAttr(const std::string &name, const bool &x)
{
	int ix = x ? 1 : 0;
	m_current->addIntAttr(name.c_str());
	m_current->writeIntAttr(name.c_str(), &ix);
}

void H5GraphIO::writeNodeStringAttr(const std::string &name, const std::string &x)
{
	m_current->addVLStringAttr(name.c_str());
	m_current->writeVLStringAttr(name.c_str(), x);
}

} /// end of alo
