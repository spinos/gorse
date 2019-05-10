/*
 *  GlobalFence.h
 *  gorse
 *
 *  global synchronization
 *
 *  2019/5/11
 */

#ifndef ALO_INTERFACE_GLOBAL_FENCE_H
#define ALO_INTERFACE_GLOBAL_FENCE_H

#include <boost/thread/lockable_adapter.hpp>
#include <boost/thread/mutex.hpp>

namespace alo {
	
namespace interface {

class GlobalFence : public boost::basic_lockable_adapter<boost::mutex> {

public:

	static GlobalFence& instance();

};
	
} /// end of inteface

} /// end of alo

#endif
