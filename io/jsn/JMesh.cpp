#include "JMesh.h"
#include <boost/foreach.hpp>

using namespace boost::property_tree;

namespace alo {

JMesh::JMesh() : m_name("unknown"),
m_nv(0), m_nt(0), m_pos(0), m_ind(0)
{}

JMesh::~JMesh()
{ m_uvsets.clear(); }

void JMesh::setName(const std::string &name)
{ m_name = name; }

void JMesh::setNvNt(int nv, int nt)
{ m_nv = nv; m_nt = nt; }

void JMesh::addUvSet(const std::string &name)
{ 
	m_uvsets.push_back(UVSet()); 
	m_uvsets.back()._name = name;
}

int JMesh::getRegionSize() const
{
    int s = 0;
    s += m_nv * 12;
    s += m_nt * 12;
    std::vector<UVSet>::const_iterator it = m_uvsets.begin();
	for(;it!=m_uvsets.end();++it)
		s += m_nt * 24;
	
	if(s & 65535) s += 65535;
	s = s>>16;
	s = s<<16;
	return s;
}

int JMesh::getPosLoc() const
{ return 0; }

int JMesh::getIndLoc() const
{ return m_nv * 12; }

ptree JMesh::createPtree()
{
	ptree tree;

	tree.put("name", m_name);
	tree.put("nv", m_nv); 
	tree.put("nt", m_nt);

	int loc = 0;
	tree.put("pos", loc);
	loc += m_nv * 12;

	tree.put("ind", loc);
	loc += m_nt * 12;

	ptree uvChildren;

	std::vector<UVSet>::const_iterator it = m_uvsets.begin();
	for(;it!=m_uvsets.end();++it) {
		ptree childUv;
		childUv.put("name", it->_name);
		childUv.put("uv", loc);
		loc += m_nt * 24;

		uvChildren.push_back(std::make_pair("", childUv));
	}

	tree.add_child("uvs", uvChildren);

	return tree;
}

bool JMesh::readPtree(const ptree & tree)
{
	if(tree.empty()) return false;

	m_name = tree.get<std::string>("name");
	m_nv = tree.get<int>("nv");
	m_nt = tree.get<int>("nt");
	m_pos = tree.get<int>("pos");
	m_ind = tree.get<int>("ind");

	BOOST_FOREACH(ptree::value_type const&v, tree.get_child("uvs")) {

		const ptree &uvtree = v.second;
		if(uvtree.empty()) {
			std::cout << "\n JMesh::readPtree empty uv";
			continue;
		}

		m_uvsets.push_back(UVSet()); 
		UVSet &auv = m_uvsets.back();
		auv._name = uvtree.get<std::string>("name");
		auv._uv = uvtree.get<int>("uv");
  	}

	return m_nv > 0;
}

const std::string &JMesh::name() const
{ return m_name; }

const int &JMesh::nv() const
{ return m_nv; }

const int &JMesh::nt() const
{ return m_nt; }

const int &JMesh::pos() const
{ return m_pos; }

const int &JMesh::ind() const
{ return m_ind; }

std::vector<JMesh::UVSet>::const_iterator JMesh::uvSetBegin() const
{ return m_uvsets.begin(); }

std::vector<JMesh::UVSet>::const_iterator JMesh::uvSetEnd() const
{ return m_uvsets.end(); }

std::ostream& operator<<(std::ostream &output, const JMesh & p)
{
	output << " mesh " << p.name()
			<< " nv " << p.nv() << " nt " << p.nt()
			<< " pos " << p.pos() << " ind " << p.ind();
	std::vector<JMesh::UVSet>::const_iterator it = p.uvSetBegin();
	for(;it!=p.uvSetEnd();++it) {
		output << " uvset " << it->_name << " uv " << it->_uv;
	}
    return output;
}

}
