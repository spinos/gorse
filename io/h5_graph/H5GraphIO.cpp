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
	
	std::cout << "\n close scene " << m_head->pathToObject()
		<< " modified at " << std::asctime(std::localtime(&tnow) );

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
	m_current->addFloatAttr(".pos", 2);
	m_current->writeFloatAttr(".pos", d);
}

void H5GraphIO::writeNodeId(int x)
{
	m_current->addIntAttr(".uid");
	m_current->writeIntAttr(".uid", &x);
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

bool H5GraphIO::openScene()
{
	std::string scenePath;
	ver1::HBase w("/");
	int nc = w.numChildren();
	for(int i=0;i<nc;++i) {
		if(!w.isChildGroup(i)) continue;
		ver1::HBase ci(w.childPath(i));

		bool hasScene = ci.hasNamedAttr(".is_graph_scene");
		ci.close();

		if(hasScene) {
			scenePath = w.childPath(i);
			break;
		}
	}
	w.close();

	if(scenePath.size() < 2)
		return false;

	m_head = new ver1::HBase(scenePath);
	int tmod;
	m_head->readIntAttr(".modified", &tmod);
	std::time_t tthen = tmod;
	std::cout << "\n open scene " << m_head->pathToObject() 
			<< " modified at " << std::asctime(std::localtime(&tthen) );
	return true;
}

void H5GraphIO::closeScene()
{
	std::cout << "\n close scene " << m_head->pathToObject();
	m_head->close();
	delete m_head;
}

void H5GraphIO::lsNodes(std::vector<std::string> &names)
{
	int nc = m_head->numChildren();
	for(int i=0;i<nc;++i) {
		if(!m_head->isChildGroup(i)) continue;
		ver1::HBase ci(m_head->childPath(i));

		bool hasNode = ci.hasNamedAttr(".is_graph_node");
		ci.close();

		if(hasNode)
			names.push_back(m_head->childPath(i));
	}
}

void H5GraphIO::openNode(const std::string &name)
{
	std::cout << "\n open node " <<name;
	m_current = new ver1::HBase(name);
}

void H5GraphIO::closeNode()
{
	std::cout << "\n close node " <<m_current->pathToObject();
	m_current->close();
	delete m_current;
}

void H5GraphIO::readNodeId(int &y)
{
	m_current->readIntAttr(".uid", &y);
}

void H5GraphIO::readNodeDisplayName(std::string &y)
{
	m_current->readVLStringAttr(".dspnm", y);
}

void H5GraphIO::readNodePosition(float *y)
{
	m_current->readFloatAttr(".pos", y);
}

} /// end of alo
