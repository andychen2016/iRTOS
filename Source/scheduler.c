#include "irtos.h"
#include "config.h"
#include "lib.h"

Bitmap 		gTaskPrioBitmap;

List 		gTaskTable[TASK_SCHED_MAX_PRIO];
Task* 		gCurrentTask;
Task* 		gNextTask;

List 		gTaskDelayList;
uint8_t 	gSchedLockCount;


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

void TaskSchedReady(Task* task)
{
    ListAddLast(&(gTaskTable[task->prio]), &(task->linkNode));
    BitmapSet(&gTaskPrioBitmap, task->prio);
}

void TaskSchedUnReady(Task* task)
{
    ListRemove(&(gTaskTable[task->prio]), &(task->linkNode));

    if(ListNodeCount(&(gTaskTable[task->prio])) == 0)
    {
        BitmapClear(&gTaskPrioBitmap, task->prio);
    }
}

void TaskSchedRemove(Task* task)
{
    ListRemove(&(gTaskTable[task->prio]), &(task->linkNode));
    
    if(ListNodeCount(&(gTaskTable[task->prio])) == 0)
    {
        BitmapClear(&gTaskPrioBitmap, task->prio);
    }
}

Task* GetHighestReady(void)
{
    uint32_t highest_prio = BitmapGetFirstSet(&gTaskPrioBitmap);  //虽然最大值为32 但是总有一个idle任务在运行
    Node* node = ListFirst(&gTaskTable[highest_prio]);
    return (Task*)ParentAddress(node,Task,linkNode);
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
    Task* tempTask;
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

        #if OS_ENABLE_HOOKS == 1
        TaskSwitchHooks(gCurrentTask, gNextTask);
        #endif

        TaskSwitch();
    }
    TaskExitCritical(status);
}

void TimeTaskWait(Task* task, uint32_t ticks)
{
    task->delaySysTick = ticks;
    ListAddLast(&gTaskDelayList, &(task->delayNode));
    task->state |= TASK_STATE_DELAYED;
}


void TimeTaskWakeUp(Task* task)
{
    ListRemove(&gTaskDelayList, &(task->delayNode));
    //ListAddLast(&gTaskTable[task->prio], &(task->linkNode));
    task->state &= ~TASK_STATE_DELAYED;
}

void TimeTaskRemove(Task* task)
{
    ListRemove(&gTaskDelayList, &(task->delayNode));
}