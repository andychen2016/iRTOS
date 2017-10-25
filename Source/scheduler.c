#include "os.h"
#include "config.h"
#include "lib.h"

tBitmap gTaskPrioBitmap;

List gTaskTable[TASK_SCHED_MAX_PRIO];
tTask* gCurrentTask;
tTask* gNextTask;

List gTaskDelayList;
uint8_t gSchedLockCount;


void TaskDelayListInit(void)
{
    ListInit(&gTaskDelayList);
}


void TaskSchedInit(void)
{
    int idx;
    gSchedLockCount = 0;
    for(idx = 0; idx < TASK_SCHED_MAX_PRIO; idx ++)
    {
        ListInit(&gTaskTable[idx]);
    }
}

void TaskSchedReady(tTask* task)
{
    ListAddLast(&(gTaskTable[task->prio]), &(task->linkNode));
    BitmapSet(&gTaskPrioBitmap, task->prio);
}

void TaskSchedUnReady(tTask* task)
{
    ListRemove(&(gTaskTable[task->prio]), &(task->linkNode));

    if(ListNodeCount(&(gTaskTable[task->prio])) == 0)
    {
        BitmapClear(&gTaskPrioBitmap, task->prio);
    }
}

void TaskSchedRemove(tTask* task)
{
    ListRemove(&(gTaskTable[task->prio]), &(task->linkNode));
    
    if(ListNodeCount(&(gTaskTable[task->prio])) == 0)
    {
        BitmapClear(&gTaskPrioBitmap, task->prio);
    }
}

tTask* GetHighestReady(void)
{
    uint32_t highestPrio = BitmapGetFirstSet(&gTaskPrioBitmap);  //虽然最大值为32 但是总有一个idle任务在运行
    Node* node = ListFirst(&gTaskTable[highestPrio]);
    return (tTask*)ParentAddress(node,tTask,linkNode);
}

void TaskSchedDisable(void)
{
    uint32_t status = TaskEnterCritical();
    if(gSchedLockCount < 255)
    {
        gSchedLockCount ++;
    }
    TaskExitCritical(status);
}

void TaskSchedEnable(void)
{
    uint32_t status = TaskEnterCritical();
    if(gSchedLockCount > 0)
    {
        if(--gSchedLockCount == 0)
        {
            TaskSched();
        }
    }
    TaskExitCritical(status);
}


void TaskSched(void)
{
    tTask* tempTask;
    uint32_t status = TaskEnterCritical();
    if(gSchedLockCount > 0)
    {
        TaskExitCritical(status);
        return;
    }

    tempTask = GetHighestReady();
    if(tempTask != gCurrentTask)
    {
        gNextTask = tempTask;
        TaskSwitch();
    }
    TaskExitCritical(status);
}

void TimeTaskWait(tTask* task, uint32_t ticks)
{
    task->delaySysTick = ticks;
    ListAddLast(&gTaskDelayList, &(task->delayNode));
    task->state |= TASK_STATE_DELAYED;
}


void TimeTaskWakeUp(tTask* task)
{
    ListRemove(&gTaskDelayList, &(task->delayNode));
    //ListAddLast(&gTaskTable[task->prio], &(task->linkNode));
    task->state &= ~TASK_STATE_DELAYED;
}

void TimeTaskRemove(tTask* task)
{
    ListRemove(&gTaskDelayList, &(task->delayNode));
}