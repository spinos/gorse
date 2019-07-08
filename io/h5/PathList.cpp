#include "PathList.h"
#include <boost/tokenizer.hpp>
#include <iostream>

namespace alo {

PathList::PathList()
{}

PathList::PathList(const std::string &name)
{
	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
	boost::char_separator<char> sep("|/");
	tokenizer tokens(name, sep);
	for (tokenizer::iterator tok_iter = tokens.begin();
		tok_iter != tokens.end(); ++tok_iter)
			m_elms.push_back(std::make_pair(std::string(*tok_iter), std::string() ) );
	
	std::string t;
	std::deque<NamePathPair>::iterator it = m_elms.begin();
	for(;it!=m_elms.end();++it) {
		t = t + "/" + it->first;
		it->second = t;
	}
}

PathList::~PathList()
{ m_elms.clear(); }

bool PathList::behead(const std::string &head)
{
	if(head.size() < 1) return false;
	
	std::deque<NamePathPair>::iterator it = m_elms.begin();
	for(;it!=m_elms.end();++it) {
		if(it->second == head) {
			break;
		}
	}
/// not found
	if(it == m_elms.end()) return false;
/// include the one matches
	it++;
/// no more left
	if(it == m_elms.end()) {
		m_elms.clear();
		return true;
	}
	m_elms.erase(m_elms.begin(), it);
	
	it = m_elms.begin();
	for(;it!=m_elms.end();++it) {
		it->second = it->second.substr(head.size());
	}

	return true;
}

void PathList::truncateBy(const PathList &another)
{
	behead(another.lastPath());
}

int PathList::length() const
{ return m_elms.size(); }

const std::string &PathList::elementName(int i) const
{ return m_elms[i].first; }

const std::string &PathList::elementPath(int i) const
{ return m_elms[i].second; }

const std::string &PathList::lastName() const
{ return m_elms.back().first; }

const std::string &PathList::lastPath() const
{ return m_elms.back().second; }

std::string PathList::lastNameStripNs() const
{ return StripNs(m_elms.back().first); }

std::string PathList::lastPathStripNs() const
{ 
	std::string t;
	std::deque<NamePathPair>::const_iterator it = m_elms.begin();
	for(;it!=m_elms.end();++it) 
		t = t + "/" + StripNs(it->first);
	
	return t;
}

std::string PathList::StripAllNs(const std::string &a)
{
	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
	boost::char_separator<char> sep("|/");
	tokenizer tokens(a, sep);
	
	std::string result;
	
	for (tokenizer::iterator tok_iter = tokens.begin();
		tok_iter != tokens.end(); ++tok_iter) {
			
		result = result + "/" + StripNs(*tok_iter);
	}
	
	return result;
}

std::string PathList::StripNs(const std::string &a)
{
	int found = a.rfind(':');

	if(found < 0) return a;
	
	std::string result = a;
	
	result.erase(0, found + 1);

	return result;
}

}
