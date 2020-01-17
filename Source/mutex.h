#ifndef __MUTEX_H__
#define __MUTEX_H__

#include "event.h"
#include "config.h"


typedef struct _tMutex
{
    Event 		event;
    uint32_t 	lockedCount;
    Task* 		ownerTask;
    uint32_t 	ownerOriginalPrio;
} Mutex;


typedef  struct _tMutexInfo
{
    uint32_t 	waitCount;
    uint32_t 	lockedCount;
    uint32_t 	ownerPrio;
    uint32_t 	inheritedPrio;
	Task*	 	ownerTask;
} MutexInfo;


void 		MutexInit(Mutex* mutex);

uint32_t 	MutexWait(Mutex* mutex, uint32_t wait_ticks);

uint32_t 	MutexNoWaitGet(Mutex* mutex);

uint32_t 	MutexNotify(Mutex* mutex);

void 		MutexGetInfo(Mutex* mutex, MutexInfo* info);

uint32_t 	MutexDestroy(Mutex* mutex);







#endif


