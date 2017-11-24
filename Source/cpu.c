#include "irtos.h"
#include "lib.h"
#include "ARMCM3.h"



uint32_t gTickCount; 

uint32_t gIdleCount;
uint32_t gIdleMaxCount;
uint32_t gIdleMaxCalculated;

float    gCpuUsage;
uint32_t gEnableCpuUsageStat;



void SetSysTickPeriod(uint32_t ms)
{
	SysTick->LOAD = ms * SystemCoreClock / 1000 - 1;
  NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
  SysTick->VAL   = 0;                           
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                   SysTick_CTRL_TICKINT_Msk   |
                   SysTick_CTRL_ENABLE_Msk; 
}


void TaskSysTick_Handler(void)
{
    Node* node;
    Task* task;
    uint32_t status = TaskEnterCritical();
	
    for(node = gTaskDelayList.headNode.nextNode; node != &(gTaskDelayList.headNode); node = node->nextNode)
    {
        task = ParentAddress(node, Task, delayNode);
        if(--task->delaySysTick == 0)
        {
            if(task->wait_event)
            {
                EventRemoveTask(task, (void*)0, ErrorTimeout);
            }
            TimeTaskWakeUp(task);
            TaskSchedReady(task);
        }
    }
		
    if(--gCurrentTask->slice == 0)
    {
        if(ListNodeCount(&(gTaskTable[gCurrentTask->prio])) > 0)
        {
            ListRemoveFirst(&(gTaskTable[gCurrentTask->prio]));
            ListAddLast(&(gTaskTable[gCurrentTask->prio]), &(gCurrentTask->linkNode));
            gCurrentTask->slice = TASK_SLICE_MAX;
        }
    }

    gTickCount ++;

    CheckCpuUsage();

    TaskExitCritical(status);

#if OS_ENABLE_TIMER == 1
    TimerModuleTickNotify();
#endif
		
    TaskSched();
}


void SysTick_Handler(void)
{
    TaskSysTick_Handler();

#if OS_ENABLE_HOOKS == 1
    SystemTick_Hooks();
#endif

}



//////////////////////
//CPU使用率测量相关

void SystemTickInit(void)
{
    gTickCount = 0;
}

void InitCpuUseageStat(void)
{
    gIdleCount = 0;
    gIdleMaxCount = 0;
    gIdleMaxCalculated = 0;
    gCpuUsage = 0.0;
    gEnableCpuUsageStat = 0;
}


void CheckCpuUsage(void)
{
    if(gEnableCpuUsageStat == 0)
    {
        gEnableCpuUsageStat = 1;
        gTickCount = 0;
        return;
    }

    if((gTickCount == OS_PER_SECOND_TICKS) && (gIdleMaxCalculated == 0))
    {
        gIdleMaxCount = gIdleCount;
        gIdleCount = 0;
        gIdleMaxCalculated = 1;

        TaskSchedEnable();
    }
    else if(gTickCount % OS_PER_SECOND_TICKS == 0)
    {
        gCpuUsage = 100.0 - (gIdleCount * 100.0) / gIdleMaxCount;
        gIdleCount = 0;
    }

}

void CpuUsageSyncWithSysTick (void)
{
    while(gEnableCpuUsageStat == 0)
    {
        ;;
    }
}

float CpuUsageGet(void)
{
    float usage = 0;

    uint32_t status = TaskEnterCritical();
    usage = gCpuUsage;
    TaskExitCritical(status);

    return usage;
}


