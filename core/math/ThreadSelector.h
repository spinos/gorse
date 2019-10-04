/*
 *  ThreadSelector.h
 *  gorse
 *
 *  N thread with id
 *
 *  2019/8/23
 */
 
#ifndef ALO_THREAD_SELECTOR_H
#define ALO_THREAD_SELECTOR_H

namespace alo {

template<int Nt>
class ThreadSelector {

    int m_threadId[Nt];
    int m_numUsed;
    
public:

    ThreadSelector();
    virtual ~ThreadSelector();
    
    void reset();
    
    bool isFull() const;
    
    int add(int x);
    void remove(int x);
    int find(int x) const;
    
};

template<int Nt>
ThreadSelector<Nt>::ThreadSelector()
{ reset(); }

template<int Nt>
ThreadSelector<Nt>::~ThreadSelector()
{}

template<int Nt>
void ThreadSelector<Nt>::reset()
{
    for(int i=0;i<Nt;++i)
        m_threadId[i] = -1;
    
    m_numUsed = 0;
}

template<int Nt>
bool ThreadSelector<Nt>::isFull() const
{ return m_numUsed == Nt; }

template<int Nt>
int ThreadSelector<Nt>::add(int x)
{
    for(int i=0;i<Nt;++i) {
        if(m_threadId[i] < 0) {
            m_threadId[i] = x;
            m_numUsed++;
            return i;
        }
    }
    return -1;
}

template<int Nt>
void ThreadSelector<Nt>::remove(int x)
{
    for(int i=0;i<Nt;++i) {
        if(m_threadId[i] == x) {
            m_threadId[i] = -1;
            m_numUsed--;
            return;
        }
    }
}

template<int Nt>
int ThreadSelector<Nt>::find(int x) const
{
    for(int i=0;i<Nt;++i) {
        if(m_threadId[i] == x) {
            return i;
        }
    }
    return -1;
}

}

#endif
