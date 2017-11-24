#include "irtos.h"
#include "sem.h"



#if OS_ENABLE_SEM == 1


void SemInit(Sem* sem, uint32_t start_count, uint32_t max_count)
{
    EventInit(&(sem->event), EventTypeSem);

    sem->max_count = max_count;
    if(max_count == 0)
    {
        sem->count = start_count;
    }
    else
    {
        sem->count = (start_count > max_count)? max_count : start_count;
    }
}

uint32_t SemWait(Sem* sem, uint32_t wait_ticks)
{
    uint32_t status = TaskEnterCritical();

    if(sem->count > 0)
    {
        --sem->count;
        TaskExitCritical(status);
        return ErrorNoError;
    }
    else
    {
        EventWait(&(sem->event), gCurrentTask, (void*)0, EventTypeSem, wait_ticks);
        TaskExitCritical(status);
        
        TaskSched();


        return gCurrentTask->wait_event_result;
    }

}

uint32_t SemNoWaitGet(Sem* sem)
{
    uint32_t status = TaskEnterCritical();

    if(sem->count > 0)
    {
        --sem->count;
        TaskExitCritical(status);
        return ErrorNoError;
    }
    else
    {
        TaskExitCritical(status);
        return ErrorResouceUnavaliable;
    }
}

uint32_t SemNotify(Sem* sem)
{
    uint32_t status = TaskEnterCritical();

    if(EventWaitCount(&(sem->event)) > 0)
    {
        tTask* task = EventWakeUp(&(sem->event), (void*)0, ErrorNoError);
        if(task->prio < gCurrentTask->prio)
        {
            TaskSched();
        }
    }
    else
    {
        ++sem->count;
        if((sem->max_count != 0) && (sem->count > sem->max_count))
        {
            sem->count = sem->max_count;
        } 
    }

    TaskExitCritical(status);

}

uint32_t SemDestroy(Sem* sem)
{
    int count = 0;
    uint32_t status = TaskEnterCritical();

    count = EventRemoveAll(&(sem->event), (void*)0, ErrorDeleted);
    sem->count = 0;
    
    TaskExitCritical(status);

    if(count > 0)
    {
        TaskSched();
    }

    return count;

}

void SemGetInfo(Sem* sem, SemInfo* info)
{
    uint32_t status = TaskEnterCritical();

    info->count = sem->count;
    info->max_count = sem->max_count;
    info->wait_count = EventWaitCount(&(sem->event));

    TaskExitCritical(status);
}


#endif


