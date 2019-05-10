/*
 *  GlobalFence.cpp
 *  gorse
 *
 *  global synchronization
 *
 *  2019/5/11
 */

#include "GlobalFence.h"

namespace alo {
	
namespace interface {

GlobalFence& GlobalFence::instance() 
{
	static GlobalFence f;
	return f;
}
	
} /// end of inteface

} /// end of alo
