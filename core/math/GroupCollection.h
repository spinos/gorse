#ifndef ALO_GROUP_COLLECTION_H
#define ALO_GROUP_COLLECTION_H

#include <map>
#include <vector>

namespace alo {

template<typename T>
class GroupCollection {

/// collect all objects
    std::map<int, T > m_elementMap;
/// indices to objects under each group
    std::map<int, std::vector<int> > m_groupMap;

public:

	GroupCollection();
	~GroupCollection();

	bool addToCollection(const T &item, int k);
	void addToGroup(int g, int k);
	const std::vector<int> &group(const int &g) const;
	const T &element(const int &k) const;

};

template<typename T>
GroupCollection<T>::GroupCollection() 
{}

template<typename T>
GroupCollection<T>::~GroupCollection() 
{
	m_elementMap.clear();
}

template<typename T>
bool GroupCollection<T>::addToCollection(const T &item, int k)
{
	if(m_elementMap.find(k) != m_elementMap.end() )
		return false;
	m_elementMap[k] = item;
	return true;
}
	
template<typename T>
void GroupCollection<T>::addToGroup(int g, int k)
{ 
	std::vector<int> &grp = m_groupMap[g];
	grp.push_back(k);
}

template<typename T>
const std::vector<int> &GroupCollection<T>::group(const int &g) const
{ return m_groupMap.at(g); }

template<typename T>
const T &GroupCollection<T>::element(const int &k) const
{ return m_elementMap.at(k); }

}

#endif
