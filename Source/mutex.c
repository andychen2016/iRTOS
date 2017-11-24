#include "irtos.h"


#if OS_ENABLE_MUTEX == 1

void MutexInit(Mutex* mutex)
{
    EventInit(&(mutex->event), EventTypeMutex);

    mutex->locked_count = 0;
    mutex->owner_task = (tTask*)0;
    mutex->owner_original_prio = TASK_SCHED_MAX_PRIO;   //

}


uint32_t MutexWait(Mutex* mutex, uint32_t wait_ticks)
{
    uint32_t status = TaskEnterCritical();

    if(mutex->locked_count == 0)
    {
        mutex->owner_task = gCurrentTask;
        mutex->owner_original_prio = gCurrentTask->prio;
        mutex->locked_count++;
        
        TaskExitCritical(status);
        return ErrorNoError;
    }
    else
    {
        if(mutex->owner_task == gCurrentTask)
        {
            mutex->locked_count ++;

            TaskExitCritical(status);
            return ErrorNoError;
        }
        else
        {
            if(gCurrentTask->prio < mutex->owner_task->prio)
            {
                tTask* task = mutex->owner_task;
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

            return gCurrentTask->wait_event_result;
        }
    }
}


uint32_t MutexNoWaitGet(Mutex* mutex)
{
    uint32_t status = TaskEnterCritical();

    if(mutex->locked_count == 0)
    {
        mutex->owner_task = gCurrentTask;
        mutex->owner_original_prio = gCurrentTask->prio;
        mutex->locked_count++;

        TaskExitCritical(status);
        return ErrorNoError;
    }
    else
    {
        if(mutex->owner_task == gCurrentTask)
        {
            mutex->locked_count++;
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

    if(mutex->locked_count == 0)
    {
        TaskExitCritical(status);
        return ErrorNoError;
    }

    if(mutex->owner_task != gCurrentTask)
    {
        TaskExitCritical(status);
        return ErrorWrongOwner;
    }

    if(--mutex->locked_count != 0)
    {
        TaskExitCritical(status);
        return ErrorNoError;
    }

    if(mutex->owner_original_prio != gCurrentTask->prio)
    {
        if(gCurrentTask->state == TASK_STATE_READY)
        {
            TaskSchedUnReady(gCurrentTask);
            gCurrentTask->prio = mutex->owner_original_prio;
            TaskSchedReady(gCurrentTask);
        }
        else
        {
            gCurrentTask->prio = mutex->owner_original_prio;
        }
    }

    if(EventWaitCount(&(mutex->event)) > 0)
    {
        tTask* task = EventWakeUp(&(mutex->event), (void*)0, ErrorNoError);

        mutex->owner_original_prio = task->prio;
        mutex->owner_task = task;
        mutex->locked_count ++;

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

    info->wait_count = EventWaitCount(&(mutex->event));
    info->locked_count = mutex->locked_count;
    info->owner_prio = mutex->owner_original_prio;
    info->inherited_prio = mutex->owner_task->prio;
    info->owner_task = mutex->owner_task;

    TaskExitCritical(status);
}


uint32_t MutexDestroy(Mutex* mutex)
{
    uint32_t count;
    uint32_t status = TaskEnterCritical();

    if(mutex->locked_count > 0)
    {
        if(mutex->owner_original_prio != mutex->owner_task->prio)
        {
            if(mutex->owner_task->state == TASK_STATE_READY)
            {
                TaskSchedUnReady(mutex->owner_task);
                mutex->owner_task->prio = mutex->owner_original_prio;
                TaskSchedReady(mutex->owner_task);
            }
            else
            {
                mutex->owner_task->prio = mutex->owner_original_prio;
            }
        }

        count = EventRemoveAll(&(mutex->event), (void*)0, ErrorDeleted);
    }

    if(count > 0) TaskSched();

    return count;
}


#endif
