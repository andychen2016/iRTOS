#include "irtos.h"


#if OS_ENABLE_MUTEX == 1

void MutexInit(Mutex* mutex)
{
    EventInit(&(mutex->event), EventTypeMutex);

    mutex->lockedCount = 0;
    mutex->ownerTask = (Task*)NULL;
    mutex->ownerOriginalPrio = TASK_SCHED_MAX_PRIO;   //

}


uint32_t MutexWait(Mutex* mutex, uint32_t wait_ticks)
{
    uint32_t status = TaskEnterCritical();

    if(mutex->lockedCount == 0)
    {
        mutex->ownerTask = gCurrentTask;
        mutex->ownerOriginalPrio = gCurrentTask->prio;
        mutex->lockedCount ++;
        
        TaskExitCritical(status);
        return ErrorNoError;
    }
    else
    {
        if(mutex->ownerTask == gCurrentTask)
        {
            mutex->lockedCount ++;

            TaskExitCritical(status);
            return ErrorNoError;
        }
        else
        {
            if(gCurrentTask->prio < mutex->ownerTask->prio)
            {
                Task* task = mutex->ownerTask;
                //在运行就绪状态，更改在就绪表中的位置
                if(task->state == TASK_STATE_READY)
                {
                    TaskSchedUnReady(task);
                    task->prio = gCurrentTask->prio;
                    TaskSchedReady(task);
                }
                else
                {
                    task->prio = gCurrentTask->prio;
                }
            }

            EventWait(&(mutex->event), gCurrentTask, (void*)0, EventTypeMutex, wait_ticks);
            
            TaskExitCritical(status);

            //切换到其他任务
            TaskSched();

            return gCurrentTask->waitEventResult;
        }
    }
}


uint32_t MutexNoWaitGet(Mutex* mutex)
{
    uint32_t status = TaskEnterCritical();

    if(mutex->lockedCount == 0)
    {
        mutex->ownerTask = gCurrentTask;
        mutex->ownerOriginalPrio = gCurrentTask->prio;
        mutex->lockedCount ++;

        TaskExitCritical(status);
        return ErrorNoError;
    }
    else
    {
        if(mutex->ownerTask == gCurrentTask)
        {
            mutex->lockedCount ++;
            TaskExitCritical(status);
            return ErrorNoError;
        }

        TaskExitCritical(status);
        return ErrorResouceUnavaliable;
    }

}

uint32_t MutexNotify(Mutex* mutex)
{
    uint32_t status = TaskEnterCritical();

    if(mutex->lockedCount == 0)
    {
        TaskExitCritical(status);
        return ErrorNoError;
    }

    if(mutex->ownerTask != gCurrentTask)
    {
        TaskExitCritical(status);
        return ErrorWrongOwner;
    }

    if(--mutex->lockedCount != 0)
    {
        TaskExitCritical(status);
        return ErrorNoError;
    }

    if(mutex->ownerOriginalPrio != gCurrentTask->prio)
    {
        if(gCurrentTask->state == TASK_STATE_READY)
        {
            TaskSchedUnReady(gCurrentTask);
            gCurrentTask->prio = mutex->ownerOriginalPrio;
            TaskSchedReady(gCurrentTask);
        }
        else
        {
            gCurrentTask->prio = mutex->ownerOriginalPrio;
        }
    }

    if(EventWaitCount(&(mutex->event)) > 0)
    {
        Task* task = EventWakeUp(&(mutex->event), (void*)0, ErrorNoError);

        mutex->ownerOriginalPrio = task->prio;
        mutex->ownerTask = task;
        mutex->lockedCount ++;

        if(task->prio < gCurrentTask->prio)
        {
            TaskSched();
        }
    }

    TaskExitCritical(status);
    return ErrorNoError;
}



void MutexGetInfo(Mutex* mutex, MutexInfo* info)
{
    uint32_t status = TaskEnterCritical();

    info->waitCount = EventWaitCount(&(mutex->event));
    info->lockedCount = mutex->lockedCount;
    info->ownerPrio = mutex->ownerOriginalPrio;
    info->inheritedPrio = mutex->ownerTask->prio;
    info->ownerTask = mutex->ownerTask;

    TaskExitCritical(status);
}


uint32_t MutexDestroy(Mutex* mutex)
{
    uint32_t count;
    uint32_t status = TaskEnterCritical();

    if(mutex->lockedCount > 0)
    {
        if(mutex->ownerOriginalPrio != mutex->ownerTask->prio)
        {
            if(mutex->ownerTask->state == TASK_STATE_READY)
            {
                TaskSchedUnReady(mutex->ownerTask);
                mutex->ownerTask->prio = mutex->ownerOriginalPrio;
                TaskSchedReady(mutex->ownerTask);
            }
            else
            {
                mutex->ownerTask->prio = mutex->ownerOriginalPrio;
            }
        }

        count = EventRemoveAll(&(mutex->event), (void*)0, ErrorDeleted);
    }

    if(count > 0) TaskSched();

    return count;
}


#endif
