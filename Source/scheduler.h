#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include <stdint.h>



extern tTask* gCurrentTask;
extern tTask* gNextTask;
extern tBitmap gTaskPrioBitmap;

extern List gTaskTable[TASK_SCHED_MAX_PRIO];
extern tTask* gCurrentTask;
extern tTask* gNextTask;

extern List gTaskDelayList;
extern uint8_t gSchedLockCount;



//Schedule
void TaskDelayListInit(void);
void TaskSchedInit(void);

void TaskSchedReady(tTask* task);
void TaskSchedUnReady(tTask* task);
void TaskSchedRemove(tTask* task);

tTask* GetHighestReady(void);

void TaskSchedDisable(void);
void TaskSchedEnable(void);

void TaskSched(void);

void TimeTaskWait(tTask* task, uint32_t ticks);
void TimeTaskWakeUp(tTask* task);
void TimeTaskRemove(tTask* task);




#endif