/*
 *  PathList.h
 *
 *  full path by name |a|b|c or /a/b/c
 *  element names [a,b,c]
 *  element paths [/a,/a/b,/a/b/c]
 *
 */

#ifndef ALO_H5_S_PATH_H
#define ALO_H5_S_PATH_H

#include <string>
#include <deque>

namespace alo {

class PathList {

typedef std::pair<std::string, std::string > NamePathPair;
	std::deque<NamePathPair> m_elms;

public:

	PathList();
	PathList(const std::string &name);
	~PathList();

/// find path of head
/// remove every element before
/// cut head from every element after
	bool behead(const std::string &head);
	
	void truncateBy(const PathList &another);

	int length() const;
	const std::string &elementName(int i) const;
	const std::string &elementPath(int i) const;

	const std::string &lastName() const;
	const std::string &lastPath() const;

	std::string lastNameStripNs() const;
	std::string lastPathStripNs() const;
	
	static std::string StripAllNs(const std::string &a);

private:
/// part after '::'
	static std::string StripNs(const std::string &a);

};

}

#endif
