/*
 *  HistoryReform.h
 *  aloe
 *
 *
 */

#ifndef ALO_HISTORY_REFORM_H
#define ALO_HISTORY_REFORM_H

namespace alo {
	
class HistoryMesh;
class AdaptableMesh;
class CoarseFineHistory;

class HistoryReform {

public:
	HistoryReform();
	~HistoryReform();

	void reform(AdaptableMesh *outMesh, HistoryMesh *stageMesh, 
					const int &selV, const int &istage, const CoarseFineHistory &stage);

protected:
/// hight to low
	void sortCoarseFaces(HistoryMesh *stageMesh, int first, int last, const int *history);

private:
/// by num vertices last before nv
	int searchCoarseBegin(int nv, int first, int last, const int *history) const;
/// first after nv
	int searchFineEnd(int nv, int first, int last, const int *history) const;

};

}

#endif
