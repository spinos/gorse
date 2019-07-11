/*
 *  SparseFieldBuilder.h
 *  gorse sds
 *
 *
 *  2019/7/11
 */

#ifndef ALO_SPARSE_FIELD_BUILDER_H
#define ALO_SPARSE_FIELD_BUILDER_H

namespace alo {
    
namespace sds {

template<typename T>
class SparseFieldBuilder {
	
	T *m_field;
	
public:

	SparseFieldBuilder();
	virtual ~SparseFieldBuilder();
	
	void attach(T *field);
	void detach();

protected:

private:	

};

template<typename T>
SparseFieldBuilder<T>::SparseFieldBuilder()
{}

template<typename T>
SparseFieldBuilder<T>::~SparseFieldBuilder()
{}
	
template<typename T>
void SparseFieldBuilder<T>::attach(T *field)
{ m_field = field; }

template<typename T>
void SparseFieldBuilder<T>::detach()
{}
	
}

}

#endif
