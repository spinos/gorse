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
	
	template<typename Tr>
	void attach(T *field, Tr &rule);
	void detach();

	template<typename Ts, typename Tr>
	void measure(Ts &samples, Tr &rule);

protected:

	T *field();

private:	

};

template<typename T>
SparseFieldBuilder<T>::SparseFieldBuilder()
{}

template<typename T>
SparseFieldBuilder<T>::~SparseFieldBuilder()
{}

template<typename T>
template<typename Tr>
void SparseFieldBuilder<T>::attach(T *field, Tr &rule)
{ 
	m_field = field; 
	m_field->create(rule.P());
	float orih[4];
	rule.getOriginCellSize(orih);
	m_field->setOriginCellSize(orih);
}

template<typename T>
void SparseFieldBuilder<T>::detach()
{
	m_field->mapNodes();
	m_field->verbose();
}

template<typename T>
template<typename Ts, typename Tr>
void SparseFieldBuilder<T>::measure(Ts &samples, Tr &rule)
{
	const int n = samples.size();
	for(int i=0;i<n;++i) {
		Ts::ValueTyp &si = samples[i];
		const float *q = (const float *)&si._pos;

		si._key = rule.computeKey(q);
	}

	samples.sort();

	int prek =-1;
	for(int i=0;i<n;++i) {
		const Ts::ValueTyp &si = samples[i];
		if(si._key != prek) {

			prek = si._key;

			rule.computeCellCornerKeys(si._key);

			m_field->addCell(rule.cellCornerKeys());

		}
	}

}

template<typename T>
T *SparseFieldBuilder<T>::field()
{ return m_field; }
	
}

}

#endif
