/*
 *  JMesh.h
 *
 *  triangle mesh property tree in json
 *  uvs are facevarying
 *
 *  {
 *		"name":"foo",
 *      "nv":100,
 *      "nt":200,
 *      "pos":0, /// location
 *      "ind":1200,
 *      "uvs":[
 *             {
 *                 "name":"map1",
 *                 "uv":2400
 *             }
 *            ]
 *  }
 *
 */

#ifndef ALO_J_MESH_H
#define ALO_J_MESH_H

#include <boost/property_tree/ptree.hpp>
#include <vector>
#include <iostream>

namespace alo {

class JMesh {

	std::string m_name;
	int m_nv, m_nt, m_pos, m_ind;

public:
	JMesh();
	~JMesh();

	void setName(const std::string &name);
	void setNvNt(int nv, int nt);
	void addUvSet(const std::string &name);

	const std::string &name() const;
	const int &nv() const;
	const int &nt() const;
	const int &pos() const;
	const int &ind() const;
/// data in bytes round to 65536
	int getRegionSize() const;
	int getPosLoc() const;
	int getIndLoc() const;

	boost::property_tree::ptree createPtree();
	bool readPtree(const boost::property_tree::ptree & tree);

	friend std::ostream& operator<<(std::ostream &output, const JMesh & p);

	struct UVSet {
		std::string _name;
		int _uv;
	};

	std::vector<UVSet>::const_iterator uvSetBegin() const;
	std::vector<UVSet>::const_iterator uvSetEnd() const;

private:
	std::vector<UVSet> m_uvsets;

};

}

#endif
