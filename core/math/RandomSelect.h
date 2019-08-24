/*
 *  RandomSelect.h
 *  gorse
 *
 *  pick one in multiple by percentage
 *  0---w[0]---w[1]---w[n-1](1)
 *  2019/8/24
 */

#ifndef ALO_RANDOM_SELECT_H
#define ALO_RANDOM_SELECT_H

namespace alo {

class RandomSelect {

	int m_val[256];
	float m_wei[256];
	int m_count;

public:

	RandomSelect();
	virtual ~RandomSelect();

	void create(int x);
	void setValue(int x, int i);
	void setWeight(float x, int i);
	void normalize();
    
    const int &count() const;

	template<typename T>
	int select(T *rng) const;

protected:

private:

};

template<typename T>
int RandomSelect::select(T *rng) const
{
	if(m_count < 2) return m_val[0];

	int r;
	for(int i=0;i<m_count;++i) {
		r = m_val[i];

		if(rng->randf1() < m_wei[i])
			return r;
	}

	return r;
}

}

#endif
