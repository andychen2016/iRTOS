#include "irtos.h"


#if OS_ENABLE_TIMER == 1

static List         gTimerHardList;

static List         gTimerSoftList;

static Sem          gTimerProtectSem;

static Sem          gTimerTickSem;

static Task         gTimerModuleTask;

static TaskStack    gTimerModuleTaskStack[TASK_STACK_SIZE];



void TimerInit(Timer* timer, uint32_t delay_ticks, uint32_t duration_ticks, 
				void (*timer_func)(void* args), void* args, uint32_t config)
{
    NodeInit(&(timer->linkNode));
    timer->startDelayTicks = delay_ticks;
    timer->durationTicks = duration_ticks;
    timer->timerFunc = timer_func;
    timer->args = args;
    timer->config = config;

    if(timer->startDelayTicks == 0)
    {
        timer->decreaseTicks = duration_ticks;
    }
    else
    {
        timer->decreaseTicks = timer->startDelayTicks;
    }
    timer->state = TimerCreated;
}






void TimerStart(Timer* timer)
{
    switch(timer->state)
    {
        case TimerCreated:
        case TimerStopped:

            timer->decreaseTicks = timer->startDelayTicks ? timer->startDelayTicks : timer->durationTicks;
            timer->state = TimerStarted;
            
            if(timer->config & TIMER_TYPE_HARD)
            {
                uint32_t status = TaskEnterCritical();
                ListAddLast(&gTimerHardList, &(timer->linkNode));
                TaskExitCritical(status);
            }
            else
            {
                SemWait(&gTimerProtectSem, 0);
                ListAddLast(&gTimerSoftList, &(timer->linkNode));
                SemNotify(&gTimerProtectSem);
            }
            break;

        default:
            break;
    }
}

void TimerStop(Timer* timer)
{
    switch(timer->state)
    {
        case TimerStarted:
        case TimerCallingFunc:
            if(timer->config & TIMER_TYPE_HARD)
            {
                uint32_t status = TaskEnterCritical();
                ListRemove(&gTimerHardList, &(timer->linkNode));
                TaskExitCritical(status);
            }
            else
            {
                SemWait(&gTimerProtectSem, 0);
                ListRemove(&gTimerSoftList, &(timer->linkNode));
                SemNotify(&gTimerProtectSem);
            }
            break;

        default:
            break;
    }
}

void TimerClose(Timer* timer)
{
    TimerStop(timer);
    timer->state = TimerClosed;
}

void TimerGetInfo(Timer* timer, TimerInfo* info)
{
    uint32_t status = TaskEnterCritical();
    
    info->startDelayTicks = timer->startDelayTicks;
    info->durationTicks = timer->durationTicks;
    info->timerFunc = timer->timerFunc;
    info->args = timer->args;
    info->state = timer->state;
    info->config = timer->config;

    TaskExitCritical(status);
}


void TimerCallFuncs(List* timer_list)
{
    Node* node;
    Node* next_node;

    for(node = timer_list->headNode.nextNode; node != &(timer_list->headNode); node = next_node)
    {
        Timer* timer = ParentAddress(node, Timer, linkNode);
        next_node = node->nextNode;

        if((timer->decreaseTicks == 0) ||(--timer->decreaseTicks == 0))
        {
            timer->state = TimerCallingFunc;

            timer->timerFunc(timer->args);

            timer->state = TimerStarted;

            if(timer->durationTicks > 0)
            {
                timer->decreaseTicks = timer->durationTicks;
            }
            else
            {
                ListRemove(timer_list, node);
                timer->state = TimerStopped;
            }
        } 
    }
}

//硬中断调用
void TimerModuleTickNotify(void)
{
    uint32_t status = TaskEnterCritical();

    TimerCallFuncs(&gTimerHardList);        //保证硬中断先运行

    TaskExitCritical(status);

    SemNotify(&gTimerTickSem);      //通知软中断开始工作
}




//本函数由时间片轮询工作
void TimerModuleSoftTask(void* param)           
{
    for(;;)
    {
        SemWait(&gTimerTickSem, 0);         //等待硬件中断通知

        SemWait(&gTimerProtectSem, 0);

        TimerCallFuncs(&gTimerSoftList);

        SemNotify(&gTimerProtectSem);

    }

}


void TimerModuleInit(void)
{
    ListInit(&gTimerHardList);
    ListInit(&gTimerSoftList);
    SemInit(&gTimerProtectSem, 1, 1);
    SemInit(&gTimerTickSem, 0, 0);
}


void TimerInitTask(void)
{
#if TASK_TIMER_MODULE_PRIO >= (TASK_SCHED_MAX_PRIO - 1)
    #error "The proprity of timer task must be greater then (TASK_SCHED_MAX_PRIO - 1)"
#endif
    TaskInit(&gTimerModuleTask, TimerModuleSoftTask, TASK_TIMER_MODULE_PRIO, (void *)0, gTimerModuleTaskStack, sizeof(gTimerModuleTaskStack));
}

#endif