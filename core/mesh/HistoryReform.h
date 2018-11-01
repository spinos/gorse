#ifndef ALO_HISTORY_REFORM_H
#define ALO_HISTORY_REFORM_H

namespace alo {
	
class HistoryMesh;
class AdaptableMesh;

class HistoryReform {

	HistoryMesh *m_mesh;

public:
	HistoryReform();

	void reform(AdaptableMesh *outMesh, const float &lod, HistoryMesh *mesh);

private:
/// hight to low
	void sortCoarseFaces(int first, int last, const int *history);
/// by num vertices last before nv
	int searchCoarseBegin(int nv, int first, int last, const int *history) const;
/// first after nv
	int searchFineEnd(int nv, int first, int last, const int *history) const;

};

}

#endif