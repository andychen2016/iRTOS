#include "irtos.h"
#include "event.h"


void TaskInit(Task* ptr_task, void (*entry)(void*), uint32_t prio, void* param, TaskStack* ptr_stack, uint32_t size)
{
    uint32_t* stack_top;

    ptr_task->stackBase = ptr_stack;
    ptr_task->stackSize = size;
    memset(ptr_task->stackBase, 0, size);

    stack_top = ptr_task->stackBase + size / sizeof(TaskStack);

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

    ptr_task->ptrStack = stack_top;
    ptr_task->delaySysTick = 0;    
    ptr_task->prio = prio;
	ptr_task->slice = TASK_SLICE_MAX;
    ptr_task->state = TASK_STATE_READY;
    ptr_task->suspendCount = 0;
    ptr_task->cleanFun = (void(*)(void*))NULL;
    ptr_task->cleanParam = (void*)NULL;
    ptr_task->requestDeleteFlag = 0;

    ptr_task->waitEvent = (Event*)NULL;
    ptr_task->eventMsg = (void*)NULL;
    ptr_task->waitEventResult = ErrorNoError;

    NodeInit(&(ptr_task->delayNode));
    NodeInit(&(ptr_task->linkNode));
    
    TaskSchedReady(ptr_task);

#if OS_ENABLE_HOOKS == 1
    TaskInitHooks(ptr_task);
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
    task->cleanFun = clean;
    task->cleanParam = param;
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

    if(task->cleanFun)
    {
        task->cleanFun(task->cleanParam);
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

    if(task->cleanFun)
    {
        task->cleanFun(task->cleanParam);
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

    info->stackFreeSize = 0;
    stack_end = task->stackBase;
    stack_start = task->stackBase + task->stackSize / sizeof(TaskStack);
    while((*stack_end++ == 0) && (stack_end <= stack_start))
    {
        info->stackFreeSize ++;
    }
    info->stackFreeSize *= sizeof(TaskStack);

    TaskExitCritical(status);
}



