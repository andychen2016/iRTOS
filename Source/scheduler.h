#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include <stdint.h>



extern Task* gCurrentTask;
extern Task* gNextTask;
extern tBitmap gTaskPrioBitmap;

extern List gTaskTable[TASK_SCHED_MAX_PRIO];
extern Task* gCurrentTask;
extern Task* gNextTask;

extern List gTaskDelayList;
extern uint8_t gSchedLockCount;



//Schedule
void TaskDelayListInit(void);
void TaskSchedInit(void);

void TaskSchedReady(Task* task);
void TaskSchedUnReady(Task* task);
void TaskSchedRemove(Task* task);

Task* GetHighestReady(void);

void TaskSchedDisable(void);
void TaskSchedEnable(void);

void TaskSched(void);

void TimeTaskWait(Task* task, uint32_t ticks);
void TimeTaskWakeUp(Task* task);
void TimeTaskRemove(Task* task);




#endif

