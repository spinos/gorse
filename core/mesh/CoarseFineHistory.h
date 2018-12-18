/*
 *  CoarseFineHistory.h
 *
 *  history to reform faces
 *  coarse and fine pair for each face
 *  fine --------->
 *             n-1,n,n+1
 *  coase          <-------
 *             n+1,n,n-1
 *  n is selected num vertices
 *  fine is increased
 *  coarse is decreased when more n is increased
 *
 */

#ifndef ALO_COARSE_FINE_HISTORY_H
#define ALO_COARSE_FINE_HISTORY_H

#include <math/SimpleBuffer.h>

namespace alo {

struct CoarseFineHistoryDesc {
	int _length;
	int _vbegin;
    int _vend;
    int _ncoarse;
    int _nfine;
    int _fbegin;
    int _fend;
};
	
class CoarseFineHistory {

	SimpleBuffer<int> m_history;
	CoarseFineHistoryDesc m_desc;

public:

	CoarseFineHistory();
	~CoarseFineHistory();
	
	void createHistory(int nf);
	void setDesc(const CoarseFineHistoryDesc &x);
	
/// create nf vert large numbers
	void initialize(int nf);
	void setVCF(const int &vbegin, const int &ncoarse, const int &nfine);
	void setNv(const int &x, int location);
	void swap(int a, int b);
/// insert n x at location
	void insert(int x, int n, int location);

	void setFRange(int x, int y);

	const int &vbegin() const;
	int &vbegin();

	const int &vend() const;
	int &vend();

	const int &nfine() const;
	const int &ncoarse() const;
	const int &fbegin() const;
	const int &fend() const;
	const int &length() const;
/// fbegin - 1
	int coarseMax() const;
/// fend - 1
	int fineMax() const;
	const CoarseFineHistoryDesc &desc() const;
	const int *c_value() const;
	int *value();

	void printDetail() const;

	void operator=(const CoarseFineHistory &b);
	friend std::ostream& operator<<(std::ostream &output, const CoarseFineHistory & p);

};

}

#endif
