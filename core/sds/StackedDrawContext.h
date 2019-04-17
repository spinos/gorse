/*
 *  StackedDrawContext.h
 *  aloe
 *
 *  collect all nodes via stack
 *
 *  Created by zhang on 18-2-19.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_SDS_STACKED_DRAW_CONTEXT_H
#define ALO_SDS_STACKED_DRAW_CONTEXT_H

#include <vector>
#include <deque>
#include <iostream>

namespace alo {

namespace sds {

template<typename T, typename Tr>
class StackedDrawContext {

	struct DrawEvent {
		float _coord[4];
	};
	
    std::vector<DrawEvent> m_drawEvents;
	std::deque<DrawEvent> m_drawQueue;
    Tr m_rule;
	const T* m_svt;
/// threshold to end subdiv
    float m_minSpan;
	
public:

	StackedDrawContext();
	
    void update(const T* svt);
    
    int numDraws() const;
    const float* drawCoord(const int& i) const;
    
private:

	void begin(const T* svt);
	bool end();
	void next();
	
	const float* currentCoord() const;
	
};

template<typename T, typename Tr>
StackedDrawContext<T, Tr>::StackedDrawContext()
{}

template<typename T, typename Tr>
void StackedDrawContext<T, Tr>::update(const T* svt)
{
    m_drawEvents.clear();
    begin(svt);
    while(!end() ) {
        
        m_drawEvents.push_back(m_drawQueue.front() );
        
        next();
    }
}

template<typename T, typename Tr>
void StackedDrawContext<T, Tr>::begin(const T* svt)
{
	m_svt = svt;
	DrawEvent e;
	memcpy(e._coord, svt->coord(), 16);
	m_rule.setCoord(svt->coord() );
	m_drawQueue.push_back(e);
    m_minSpan = svt->minSubdivSpan();
}

template<typename T, typename Tr>
bool StackedDrawContext<T, Tr>::end()
{ return m_drawQueue.size() < 1; }

template<typename T, typename Tr>
void StackedDrawContext<T, Tr>::next()
{
	const DrawEvent& fe = m_drawQueue.front();
	
    const float& halfSpan = fe._coord[3];
    
    if(halfSpan < m_minSpan) {
        m_drawQueue.pop_front();
        return;
    }
    
    float origin[3];
    origin[0] = fe._coord[0] - halfSpan;
    origin[1] = fe._coord[1] - halfSpan;
    origin[2] = fe._coord[2] - halfSpan;
    
    const int d = T::NumSubdivPerDim();
    const float hh = halfSpan / (float)d;
    const float h = hh * 2.f;
    
    DrawEvent subEvent;
    for(int k=0;k<d;++k) {
        for(int j=0;j<d;++j) {
            for(int i=0;i<d;++i) {
            
                subEvent._coord[0] = origin[0] + h * (i + .5f);
                subEvent._coord[1] = origin[1] + h * (j + .5f);
                subEvent._coord[2] = origin[2] + h * (k + .5f);
                subEvent._coord[3] = hh;
                
                int subk = m_rule.computeKey(subEvent._coord);
                
                if(m_svt->findNode(subk) > -1 ) {
                    m_drawQueue.push_back(subEvent);
                }
            }
        }
    }
	
	m_drawQueue.pop_front();
}

template<typename T, typename Tr>
const float* StackedDrawContext<T, Tr>::currentCoord() const
{ 
	const DrawEvent& fe = m_drawQueue.front();
	return fe._coord;
}

template<typename T, typename Tr>
int StackedDrawContext<T, Tr>::numDraws() const
{ return m_drawEvents.size(); }

template<typename T, typename Tr>
const float* StackedDrawContext<T, Tr>::drawCoord(const int& i) const
{ return m_drawEvents[i]._coord; }


}

}

#endif
