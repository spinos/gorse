/*
 *  GraphUtil.h
 *  aloe
 *
 *  2019/9/25
 */
 
#ifndef ALO_GRAPH_UTIL_H
#define ALO_GRAPH_UTIL_H

#include <vector>
 
namespace alo {
    
class GraphUtil {
  
public:

    template<typename Tm, typename Tb>
	static void MapVertexEdgeIndex(std::vector<int> &edgeBegins,
					std::vector<int> &edgeInds,
					Tm &edgeMap);
                    
private:
                    
	static void PushIndices(const std::vector<int> & a,
					std::vector<int> & b);
    
};

template<typename Tm, typename Tb>
void GraphUtil::MapVertexEdgeIndex(std::vector<int> &edgeBegins,
					std::vector<int> &edgeInds,
					Tm &edgeMap)
{
	std::map<int, std::vector<int> > vvemap;

	int c = 0;
	Tb *block = edgeMap.begin();
	while(block) {
		for (int i=0;i<block->count();++i) { 
			const sdb::Coord2 &edgeI = block->key(i);
			int v0 = edgeI.x;
			vvemap[v0].push_back(c);
		
			int v1 = edgeI.y;
			vvemap[v1].push_back(c);
		
			c++;
		}
		block = edgeMap.next(block);
	}

	int nvve = 0;
	std::map<int, std::vector<int> >::iterator it = vvemap.begin();
	for(;it!=vvemap.end();++it) {
		edgeBegins.push_back(nvve);
		
		PushIndices(it->second, edgeInds);
		nvve += (it->second).size();
		
		it->second.clear();
	}
}

}

#endif
