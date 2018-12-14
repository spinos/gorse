/*
 *  LodMeshGroup.h
 *  aloe
 *
 *  many mesh caches in one file
 *
 */

#ifndef ALO_LOD_MESH_GROUP_H
#define ALO_LOD_MESH_GROUP_H

#include <vector>
#include <string>

namespace alo {

class LodMeshCache;
class BoundingBox;

class LodMeshGroup {
	
	std::string m_cacheFilePath;
	std::vector<LodMeshCache *> m_cacheList;

public:
	LodMeshGroup();
	virtual ~LodMeshGroup();

	bool loadMeshes(const std::string &fileName);

	const std::string &cacheFilePath() const;
	int numMeshes() const;
	LodMeshCache *mesh(int i);

	void getMeshAabb(BoundingBox &box, int i) const;

protected:

private:
	void clear();
	
};

}

#endif
