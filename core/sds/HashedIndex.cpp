/*
 *  HashedIndex.cpp
 *  aloe
 *
 *  Created by zhang on 18-2-18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#include "HashedIndex.h"
#include <math/miscfuncs.h>

namespace alo {

namespace sds {

HashedIndex::HashedIndex() : m_tableSize(0)
{}

HashedIndex::~HashedIndex()
{}

void HashedIndex::create(int n)
{
/// more than n elements
    m_tableSize = DivideUp(n * 2, 128) * 128;
    m_table.reset(new int[m_tableSize]);
    m_index.reset(new int[m_tableSize]);
    
    for(int i=0;i<m_tableSize;++i) {
        m_table[i] = -1;
    }
}

const int& HashedIndex::tableSize() const
{ return m_tableSize; }

int HashedIndex::insertKey(const int& k)
{
    int rk = k;
    int ntest = 0;
    int forbiden = -1;
    while(forbiden < 0) {
        const int l = hashFunc(rk) % m_tableSize;
        if(m_table[l] == k) {
			forbiden = l;
		} else if(m_table[l] < 0) {
            m_table[l] = k;
            forbiden = l;
        } else {
/// offset
            rk += 2;
        }
        ntest++;
        
        if(ntest > 18)
            break;
    }
    return forbiden;
}

int HashedIndex::finish()
{
    int l = 0;
    for(int i=0;i<m_tableSize;++i) {
        if(m_table[i] > -1) {
            m_index[i] = l;
            l++;
        }
    }
    return l;
}

int HashedIndex::lookupKey(const int& k) const
{
    int rk = k;
    int ntest = 0;
    int matched = -1;
    while(matched < 0) {
        const int l = hashFunc(rk) % m_tableSize;
        if(m_table[l] == k) {
            matched = l;
        } else {
/// offset
            rk += 2;
        }
        ntest++;
        
        if(ntest > 18)
            break;
    }
    if(matched < 0)
        return matched;
        
    return m_index[matched];
}

int HashedIndex::hashFunc(const int& x) const
{
    int key = x;
    key += ~(key << 15);
		key ^=  (key >> 10);
		key +=  (key << 3);
		key ^=  (key >> 6);
		key += ~(key << 11);
		key ^=  (key >> 16);
    return key;
}

const int* HashedIndex::table() const
{ return m_table.get(); }

const int* HashedIndex::index() const
{ return m_index.get(); }

int* HashedIndex::tableR()
{ return m_table.get(); }

int* HashedIndex::indexR()
{ return m_index.get(); }

}

}
