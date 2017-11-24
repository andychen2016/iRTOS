#include "irtos.h"


#if OS_ENABLE_TIMER == 1

static List         gTimerHardList;

static List         gTimerSoftList;

static Sem          gTimerProtectSem;

static Sem          gTimerTickSem;

static Task         gTimerModuleTask;
static TaskStack    gTimerModuleTaskStack[TASK_STACK_SIZE];



void TimerInit(Timer* timer, uint32_t delay_ticks, uint32_t duration_ticks, 
    void (*timerfunc)(void* arg), void* arg, uint32_t config)
{
    NodeInit(&(timer->link_node));
    timer->start_delay_ticks = delay_ticks;
    timer->duration_ticks = duration_ticks;
    timer->timer_func = timerfunc;
    timer->arg = arg;
    timer->config = config;

    if(timer->start_delay_ticks == 0)
    {
        timer->decrease_ticks = duration_ticks;
    }
    else
    {
        timer->decrease_ticks = timer->start_delay_ticks;
    }
    timer->state = TimerCreated;
}






void TimerStart(Timer* timer)
{
    switch(timer->state)
    {
        case TimerCreated:
        case TimerStopped:

            timer->decrease_ticks = timer->start_delay_ticks ? timer->start_delay_ticks : timer->duration_ticks;
            timer->state = TimerStarted;
            
            if(timer->config & TIMER_TYPE_HARD)
            {
                uint32_t status = TaskEnterCritical();
                ListAddLast(&gTimerHardList, &(timer->link_node));
                TaskExitCritical(status);
            }
            else
            {
                SemWait(&gTimerProtectSem, 0);
                ListAddLast(&gTimerSoftList, &(timer->link_node));
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
                ListRemove(&gTimerHardList, &(timer->link_node));
                TaskExitCritical(status);
            }
            else
            {
                SemWait(&gTimerProtectSem, 0);
                ListRemove(&gTimerSoftList, &(timer->link_node));
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
    
    info->start_delay_ticks = timer->start_delay_ticks;
    info->duration_ticks = timer->duration_ticks;
    info->timer_func = timer->timer_func;
    info->arg = timer->arg;
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
        Timer* timer = ParentAddress(node, Timer, link_node);
        next_node = node->nextNode;

        if((timer->decrease_ticks == 0) ||(--timer->decrease_ticks == 0))
        {
            timer->state = TimerCallingFunc;

            timer->timer_func(timer->arg);

            timer->state = TimerStarted;

            if(timer->duration_ticks > 0)
            {
                timer->decrease_ticks = timer->duration_ticks;
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
    uint32_t status =TaskEnterCritical();

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