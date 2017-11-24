#include "irtos.h"
#include "event.h"


void TaskInit(Task* task, void (*entry)(void*), uint32_t prio, void* param, TaskStack* pStack, uint32_t size)
{
    uint32_t* stack_top;

    task->stack_base = pStack;
    task->stack_size = size;
    memset(task->stack_base, 0, size);

    stack_top = task->stack_base + size / sizeof(TaskStack);

    *(--stack_top) = (unsigned long)(0x1 << 24);
    *(--stack_top) = (unsigned long)(entry);
    *(--stack_top) = (unsigned long)0x14;
    *(--stack_top) = (unsigned long)0x12;
    *(--stack_top) = (unsigned long)0x3;
    *(--stack_top) = (unsigned long)0x2;
    *(--stack_top) = (unsigned long)0x1;
    *(--stack_top) = (unsigned long)param;

    *(--stack_top) = (unsigned long)0x11;
    *(--stack_top) = (unsigned long)0x10;
    *(--stack_top) = (unsigned long)0x9;
    *(--stack_top) = (unsigned long)0x8;
    *(--stack_top) = (unsigned long)0x7;
    *(--stack_top) = (unsigned long)0x6;
    *(--stack_top) = (unsigned long)0x5;
    *(--stack_top) = (unsigned long)0x4;

    task->pStack = stack_top;
    task->delaySysTick = 0;    
    task->prio = prio;
	task->slice = TASK_SLICE_MAX;
    task->state = TASK_STATE_READY;
    task->suspendCount = 0;
    task->clean_fun = (void(*)(void*))0;
    task->clean_param = (void*)0;
    task->requestDeleteFlag = 0;

    task->wait_event = (Event*)0;
    task->event_msg = (void*)0;
    task->wait_event_result = ErrorNoError;

    NodeInit(&(task->delayNode));
    NodeInit(&(task->linkNode));
    
    TaskSchedReady(task);

#if OS_ENABLE_HOOKS == 1
    TaskInit_Hooks(task);
#endif
}

void TaskSuspend(Task* task)
{
    uint32_t status = TaskEnterCritical();

    if(!(task->state & TASK_STATE_DELAYED))
    {   
        if(task->suspendCount++ < 1)
        {
            task->state |= TASK_STATE_SUSPEND;
            TaskSchedUnReady(task);
            if(task == gCurrentTask)
            {
                TaskSched();            
            }
        }
    }

    TaskExitCritical(status);
}

void TaskWakeUp(Task* task)
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


void TaskSetCleanFunc(Task* task, void(*clean)(void*), void* param)
{
    task->clean_fun = clean;
    task->clean_param = param;
}

void TaskForceDelete(Task* task)
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
        task->state |= TASK_STATE_DELETED;
    }

    if(gCurrentTask == task)
    {
        TaskSched();
    }

    TaskExitCritical(status);
}

void TaskRequestDelete(Task* task)
{
    uint32_t status = TaskEnterCritical();

    task->requestDeleteFlag = 1;

    TaskExitCritical(status);
}

uint8_t TaskIsRequestDelete(Task* task)
{
    uint8_t delFlag;
    uint32_t status = TaskEnterCritical();

    delFlag = task->requestDeleteFlag;

    TaskExitCritical(status);
    return delFlag;
}

void TaskDeleteSelf(Task* task)
{
    uint32_t status = TaskEnterCritical();
    
    TaskSchedRemove(task);

    if(task->clean_fun)
    {
        task->clean_fun(task->clean_param);
        task->state |= TASK_STATE_DELETED;
    }

    TaskSched();

    TaskExitCritical(status);
}


void TaskGetInfo(Task* task, TaskInfo* info)
{
    uint32_t *stack_end;
    uint32_t *stack_start;
    uint32_t status = TaskEnterCritical();

    info->delaySysTick = task->delaySysTick;
    info->slice = task->slice;
    info->suspendCount = task->suspendCount;
    info->prio = task->prio;

    info->stack_free_size = 0;
    stack_end = task->stack_base;
    stack_start = task->stack_base + task->stack_size / sizeof(TaskStack);
    while((*stack_end++ == 0) && (stack_end <= stack_start))
    {
        info->stack_free_size ++;
    }
    info->stack_free_size *= sizeof(TaskStack);

    TaskExitCritical(status);
}



