#include "delays.h"
#include <stdint.h>
#include "timer16.h"
#include "timer6.h"
#include "core.h"

// warnig appropriate queues needs to be initialized

void usDelay(uint64_t Delay){
 usDelayTime = 0; 
 while(usDelayTime < Delay){
 }
}