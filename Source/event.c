#include "irtos.h"
#include "event.h"




void EventInit(Event* event, EventType type)
{
    event->event_type = type;
    ListInit(&(event->wait_list));
}

void EventWait(Event* event, Task* task, void* msg, uint32_t state, uint32_t timeout)
{
    uint32_t status = TaskEnterCritical();

    task->state |= (state << 16);
    task->wait_event = event;
    task->event_msg = msg;
    task->wait_event_result = ErrorNoError;

    TaskSchedUnReady(task);

    ListAddLast(&(event->wait_list), &(task->linkNode));   //将linkNode加入wait_list

    if(timeout)
    {
        TimeTaskWait(task, timeout);        //将delayNode加入gTaskDelayList
    }

    TaskSched();        //等待事件后，应该直接切换到其他任务；

    TaskExitCritical(status);
}

Task* EventWakeUp(Event* event, void* msg, uint32_t result)
{
    Node* node;
    Task* task = (Task*)0;

    uint32_t status = TaskEnterCritical();

    if((node = ListRemoveFirst(&event->wait_list)) != (Node *)0)
    {
        task = (Task*)ParentAddress(node, Task, linkNode);

        task->wait_event = (Event*)0;
        task->event_msg = msg;
        task->wait_event_result = result;

        task->state &= ~TASK_WAIT_MASK;

        if(task->delaySysTick != 0)
        {
            TimeTaskWakeUp(task);
        }

        TaskSchedReady(task);
        TaskSched();        //等待事件后，应该直接切换到其他任务；
    }

    TaskExitCritical(status);

    return task;
}

void EventWakeUpTask(Event* event, Task* task, void* msg, uint32_t result)
{
    uint32_t status = TaskEnterCritical();

    ListRemove(&(event->wait_list), &(task->linkNode));

    task->wait_event = (Event*)0;
    task->event_msg = msg;
    task->wait_event_result = result;
    task->state &= ~TASK_WAIT_MASK;

    if(task->delaySysTick != 0)
    {
        TimeTaskWakeUp(task);
    }

    TaskSchedReady(task);    //等待事件后，应该直接切换到其他任务；
    
    TaskExitCritical(status);
}

void EventRemoveTask(Task* task, void* msg, uint32_t result)
{
    uint32_t status = TaskEnterCritical();

    ListRemove(&(task->wait_event->wait_list), &(task->linkNode));

    task->wait_event = (Event*)0;
    task->event_msg = msg;
    task->wait_event_result = result;

    task->state &= ~TASK_WAIT_MASK;

    TaskExitCritical(status);
}

uint32_t EventRemoveAll(Event* event, void* msg, uint32_t result)
{
    Node* node;
    uint32_t count;
    uint32_t status = TaskEnterCritical();

    count = ListNodeCount(&(event->wait_list));

    while((node = ListRemoveFirst(&(event->wait_list))) != (Node*)0)
    {
        Task* task = (Task*)ParentAddress(node, Task, linkNode);

        task->wait_event = (Event*)0;
        task->wait_event_result = result;
        task->event_msg = msg;
        task->state &= ~TASK_WAIT_MASK;     //屏蔽掉所有事件状体

        if(task->delaySysTick != 0)
        {
            TimeTaskWakeUp(task);
        }

        TaskSchedReady(task);
    }

    TaskExitCritical(status);

    return count;
		
}

uint32_t EventWaitCount(Event* event)
{
    uint32_t count = 0;
    uint32_t status = TaskEnterCritical();

    count = ListNodeCount(&(event->wait_list));

    TaskExitCritical(status);
    
    return count;
}



