#include "os.h"


void TaskInit(tTask* pTask, void (*entry)(void*), uint32_t prio, void* param, tTaskStack* pStack)
{
    *(--pStack) = (unsigned long)(0x1 << 24);
    *(--pStack) = (unsigned long)(entry);
    *(--pStack) = (unsigned long)0x14;
    *(--pStack) = (unsigned long)0x12;
    *(--pStack) = (unsigned long)0x3;
    *(--pStack) = (unsigned long)0x2;
    *(--pStack) = (unsigned long)0x1;
    *(--pStack) = (unsigned long)param;

    *(--pStack) = (unsigned long)0x11;
    *(--pStack) = (unsigned long)0x10;
    *(--pStack) = (unsigned long)0x9;
    *(--pStack) = (unsigned long)0x8;
    *(--pStack) = (unsigned long)0x7;
    *(--pStack) = (unsigned long)0x6;
    *(--pStack) = (unsigned long)0x5;
    *(--pStack) = (unsigned long)0x4;

    pTask->pStack = pStack;
    pTask->delaySysTick = 0;    
    pTask->prio = prio;
		pTask->slice = TASK_SLICE_MAX;
    pTask->state = TASK_STATE_READY;
    pTask->suspendCount = 0;
    pTask->clean_fun = (void(*)(void*))0;
    pTask->clean_param = (void*)0;
    pTask->requestDeleteFlag = 0;

    pTask->wait_event = (Event*)0;
    pTask->event_msg = (void*);
    pTask->wait_event_result = ErrorNoError;

    NodeInit(&(pTask->delayNode));
    NodeInit(&(pTask->linkNode));
    ListAddLast(&(gTaskTable[prio]), &(pTask->linkNode));
    BitmapSet(&gTaskPrioBitmap, prio);
}

void TaskSuspend(tTask* task)
{
    uint32_t status = TaskEnterCritical();

    if(!(task->state & TASK_STATE_DELAYED))
    {   
        if(task->suspendCount++ < 1)
        {
            task->state != TASK_STATE_SUSPEND;
            TaskSchedUnReady(task);
            if(task == gCurrentTask)
            {
                TaskSched();            
            }
        }
    }

    TaskExitCritical(status);
}

void TaskWakeUp(tTask* task)
{
    uint32_t status = TaskEnterCritical();

    if(task->state & TASK_STATE_SUSPEND)
    {
        if(--task->suspendCount == 0)
        {
            task->state &= ~TASK_STATE_SUSPEND;
            TaskSchedReady(task);
            if(task == gCurrentTask)
            {
                TaskSched();
            }
        }
    }

    TaskExitCritical(status);
}


void TaskSetCleanFunc(tTask* task, void(*clean)(void*), void* param)
{
    task->clean_fun = clean;
    task->clean_param = param;
}

void TaskForceDelete(tTask* task)
{
    uint32_t status = TaskEnterCritical();

    if(task->state & TASK_STATE_DELAYED)
    {
        TimeTaskRemove(task);
    }
    else if(!(task->state & TASK_STATE_SUSPEND))
    {
        TaskSchedRemove(task);
    }

    if(task->clean_fun)
    {
        task->clean_fun(task->clean_param);
        task->state != TASK_STATE_DELETED;
    }

    if(gCurrentTask == task)
    {
        TaskSched();
    }

    TaskExitCritical(status);
}

void TaskRequestDelete(tTask* task)
{
    uint32_t status = TaskEnterCritical();

    task->requestDeleteFlag = 1;

    TaskExitCritical(status);
}

uint8_t TaskIsRequestDelete(tTask* task)
{
    uint8_t delFlag;
    uint32_t status = TaskEnterCritical();

    delFlag = task->requestDeleteFlag;

    TaskExitCritical(status);
    return delFlag;
}

void TaskDeleteSelf(tTask* task)
{
    uint32_t status = TaskEnterCritical();
    
    TaskSchedRemove(task);

    if(task->clean_fun)
    {
        task->clean_fun(task->clean_param);
        task->state != TASK_STATE_DELETED;
    }

    TaskSched();

    TaskExitCritical(status);
}


void TaskGetInfo(tTask* task, TaskInfo* info)
{
    uint32_t status = TaskEnterCritical();

    info->delaySysTick = task->delaySysTick;
    info->slice = task->slice;
    info->suspendCount = task->suspendCount;
    info->prio = task->prio;

    TaskExitCritical(status);
}



