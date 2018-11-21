/*
 *  HCoarseFineHistory.h
 *  aloe
 *
 *  attr[7] length vbegin vend ncoarse nfine fbegin fend
 *  .his int[length]
 *
 */

#ifndef ALO_H_COARSE_FINE_HISTORY_H
#define ALO_H_COARSE_FINE_HISTORY_H

#include <h5/V1HBase.h>

namespace alo {

struct CoarseFineHistoryDesc;
class CoarseFineHistory;

class HCoarseFineHistory : public ver1::HBase {

public:
    HCoarseFineHistory(const std::string & path);
    virtual ~HCoarseFineHistory();
    
    virtual bool save(const CoarseFineHistory *his);
/// desc only
    virtual bool load(CoarseFineHistoryDesc *desc);
	
private:
	
};

}

#endif
