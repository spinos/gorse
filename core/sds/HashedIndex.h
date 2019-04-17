/*
 *  HashedIndex.h
 *  aloe
 *
 *  map coded location into memory location
 *  IndexOf(k) <- H(k)
 *  input k is scrambled using Thomas Wang's hash
 *  hash collision is handled by looking for new key by offset 2
 *
 *  Created by zhang on 18-2-18.
 *  Copyright 2018 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ALO_SDS_HASHED_INDEX_H
#define ALO_SDS_HASHED_INDEX_H

#include <boost/scoped_array.hpp>

namespace alo {

namespace sds {

class HashedIndex {

    boost::scoped_array<int> m_table;
    boost::scoped_array<int> m_index;
    int m_tableSize;
    
public:

    HashedIndex();
    ~HashedIndex();
    
/// n elements
    void create(int n);
/// into table 
/// return location in table
/// -1 if failed
    int insertKey(const int& k);
/// scan index
/// return number of occupied table 
    int finish();
/// location of k in table
/// -1 if failed
    int lookupKey(const int& k) const;
	
	const int& tableSize() const;
	const int* table() const;
	const int* index() const;
	int* tableR();
	int* indexR();
    
protected:

    int hashFunc(const int& x) const;

private:

};

}

}

#endif
