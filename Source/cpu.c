#include "os.h"
#include "lib.h"
#include "ARMCM3.h"


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
    tTask* task;
    uint32_t status = TaskEnterCritical();
	
    for(node = gTaskDelayList.headNode.nextNode; node != &(gTaskDelayList.headNode); node = node->nextNode)
    {
        task = ParentAddress(node, tTask, delayNode);
        if(--task->delaySysTick == 0)
        {
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
    TaskExitCritical(status);

    TaskSched();
}


void SysTick_Handler(void)
{
    TaskSysTick_Handler();
}

