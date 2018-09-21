#ifndef ALO_ARRAY_3_H
#define ALO_ARRAY_3_H

#include "Array2.h"

namespace alo {

/// http://www.owlnet.rice.edu/~ceng303/manuals/fortran/FOR5_3.html
/// m-by-n-by-p array
template<typename T>
struct Array3 {
	
	Array2<T> * m_slice;
	int m_P;
	
	Array3() {
		m_slice = NULL;
		m_P = 0;
	}
	
	Array3(const Array3 & another) {
		m_slice = NULL;
		m_P = 0;
		copy(another);
	}
	
	~Array3() {
		if(m_P) delete[] m_slice;
	}
	
	void create(const int & m, const int & n, const int & p = 1) {
		if(m_P < p) {
			if(m_P) delete[] m_slice;
			m_slice = new Array2<T>[p];	
		}
		
		for(int i=0; i<p; ++i)
			m_slice[i].create(m,n);
		
		m_P = p;
	}

	const int & numRows() const {
		return m_slice[0].numRows();
	}
	
	const int & numCols() const {
		return m_slice[0].numCols();
	}
	
	const int & numRanks() const {
		return m_P;
	}
	
/// i-th rank
	const Array2<T> * rank(const int & i) const {
		return &m_slice[i];
	}

	Array2<T> * rank(const int & i) {
		return &m_slice[i];
	}
	
	void copy(const Array3<T> & another) {
		create(another.numRows(), another.numCols(),
				another.numRanks() );
		for(int i=0;i<another.numRanks();++i) {
			rank(i)->copy(*another.rank(i) );
		}
	}
	
	void operator=(const Array3<T> & another) {
		copy(another);
	}
	
	void setZero();
	
	void sample(const Array3<T> & another, 
		const Array2SampleProfile<T> * prof);
};

template<typename T>
void Array3<T>::setZero()
{
	for(int i=0;i<m_P;++i) {
		m_slice[i].setZero();
	}
}

template<typename T>
void Array3<T>::sample(const Array3<T> & another, 
		const Array2SampleProfile<T> * prof)
{
	for(int i=0;i<m_P;++i) {
		m_slice[i].sample(*another.rank(i), prof);
	}
}

}
#endif        //  #ifndef APHID_TYPES_H
