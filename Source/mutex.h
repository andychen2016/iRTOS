#ifndef __MUTEX_H__
#define __MUTEX_H__

#include "event.h"
#include "config.h"


typedef struct _tMutex
{
    Event event;
    uint32_t locked_count;
    Task* owner_task;
    uint32_t owner_original_prio;
} Mutex;

typedef  struct _tMutexInfo
{
    uint32_t wait_count;
    uint32_t locked_count;
    uint32_t owner_prio;
    uint32_t inherited_prio;

    Task* owner_task;
} MutexInfo;


void MutexInit(Mutex* mutex);

uint32_t MutexWait(Mutex* mutex, uint32_t wait_ticks);

uint32_t MutexNoWaitGet(Mutex* mutex);

uint32_t MutexNotify(Mutex* mutex);

void MutexGetInfo(Mutex* mutex, MutexInfo* info);

uint32_t MutexDestroy(Mutex* mutex);







#endif


