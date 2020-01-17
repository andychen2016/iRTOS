#include "irtos.h"
#include "event.h"




void EventInit(Event* event, EventType type)
{
    event->eventType = type;
    ListInit(&(event->waitList));
}

void EventWait(Event* event, Task* task, void* msg, uint32_t state, uint32_t timeout)
{
    uint32_t status = TaskEnterCritical();

    task->state |= (state << 16);
    task->waitEvent = event;
    task->eventMsg = msg;
    task->waitEventResult = ErrorNoError;

    TaskSchedUnReady(task);

    ListAddLast(&(event->waitList), &(task->linkNode));   //将linkNode加入wait_list

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
    Task* task = (Task*)NULL;

    uint32_t status = TaskEnterCritical();

    if((node = ListRemoveFirst(&event->waitList)) != (Node *)0)
    {
        task = (Task*)ParentAddress(node, Task, linkNode);

        task->waitEvent = (Event*)0;
        task->eventMsg = msg;
        task->waitEventResult = result;

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

    ListRemove(&(event->waitList), &(task->linkNode));

    task->waitEvent = (Event*)0;
    task->eventMsg = msg;
    task->waitEventResult = result;
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

    ListRemove(&(task->waitEvent->waitList), &(task->linkNode));

    task->waitEvent = (Event*)0;
    task->eventMsg = msg;
    task->waitEventResult = result;

    task->state &= ~TASK_WAIT_MASK;

    TaskExitCritical(status);
}

uint32_t EventRemoveAll(Event* event, void* msg, uint32_t result)
{
    Node* node;
    uint32_t count;
    uint32_t status = TaskEnterCritical();

    count = ListNodeCount(&(event->waitList));

    while((node = ListRemoveFirst(&(event->waitList))) != (Node*)0)
    {
        Task* task = (Task*)ParentAddress(node, Task, linkNode);

        task->waitEvent = (Event*)0;
        task->waitEventResult = result;
        task->eventMsg = msg;
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

    count = ListNodeCount(&(event->waitList));

    TaskExitCritical(status);
    
    return count;
}



