#ifndef ALO_SSDF_MULTI_LOOKUP_RULE_H
#define ALO_SSDF_MULTI_LOOKUP_RULE_H

#include "SsdfLookupRule.h"
#include <math/Int2.h>

namespace alo {
	
namespace sdf {

template<typename T>
class MultiLookupRule {

	T *m_fields;
	typedef sdf::SsdfLookupRule<T> LookupRuleTyp;
	LookupRuleTyp *m_rules;
	int m_numFields;

public:
	
	MultiLookupRule();
	virtual ~MultiLookupRule();

	void create(int n);

	void clear();

	bool isEmpty() const;

	T &field(int i);
	void attach(int i);

	void setAllRelativeBoundaryOffset(float x);
/// find the closest one
	float mapDistance(const float *q, 
		const int *inds, const Int2 &range, 
		int &closestObjectId) const;

	void mapNormal(Vector3F &nml,
		const float *q, const int &objectId) const;

protected:

private:

};

template<typename T>
MultiLookupRule<T>::MultiLookupRule() : m_numFields(0)
{}

template<typename T>
MultiLookupRule<T>::~MultiLookupRule()
{}

template<typename T>
void MultiLookupRule<T>::create(int n)
{
	m_fields = new T[n];
	m_rules = new LookupRuleTyp[n];
	m_numFields = n;
}

template<typename T>
void MultiLookupRule<T>::clear()
{
	if(m_numFields < 1) return;
	m_numFields = 0;
	delete[] m_fields;
	delete[] m_rules;
}

template<typename T>
bool MultiLookupRule<T>::isEmpty() const
{ return m_numFields < 1; }

template<typename T>
T &MultiLookupRule<T>::field(int i)
{ return m_fields[i]; }

template<typename T>
void MultiLookupRule<T>::attach(int i)
{ m_rules[i].attach(m_fields[i]); }

template<typename T>
void MultiLookupRule<T>::setAllRelativeBoundaryOffset(float x)
{
    if(m_numFields<1) return;

    for(int i=0;i<m_numFields;++i) {
        m_rules[i].setRelativeBoundaryOffset(x);
    }
}

template<typename T>
float MultiLookupRule<T>::mapDistance(const float *q, 
				const int *inds, const Int2 &range, 
				int &closestObjectId) const
{
	float md = 1e10f;
    for(int i=range.x;i<range.y;++i) {
        const int &objI = inds[i];

        float d = m_rules[objI].lookup(q);
        if(md > d) {
            md = d;
            closestObjectId = objI;
        }
    }
    return md;
}

template<typename T>
void MultiLookupRule<T>::mapNormal(Vector3F &nml,
		const float *q, const int &objectId) const
{
	nml = m_rules[objectId].lookupNormal(q);
}

}

}

#endif
