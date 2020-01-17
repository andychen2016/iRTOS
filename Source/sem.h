#ifndef __SEM_H__
#define __SEM_H__

#include "event.h"

typedef struct _tSem
{
    Event 		event;
    uint32_t    count;
    uint32_t    maxCount;
} 	Sem;

typedef struct _tSemInfo
{
    uint32_t 	count;
    uint32_t 	maxCount;
    uint32_t 	waitCount;
} 	SemInfo;

void SemInit(Sem* sem, uint32_t start_count, uint32_t max_count);

uint32_t SemWait(Sem* sem, uint32_t wait_ticks);

uint32_t SemNoWaitGet(Sem* sem);

uint32_t SemNotify(Sem* sem);

uint32_t SemDestroy(Sem* sem);

void SemGetInfo(Sem* sem, SemInfo* info);


#endif


