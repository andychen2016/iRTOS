#include "irtos.h"


void TaskDelay(uint32_t delay_sys_tick)
{
    uint32_t status = TaskEnterCritical();
	
    TimeTaskWait(gCurrentTask, delay_sys_tick);
    TaskSchedUnReady(gCurrentTask);
    	
    TaskExitCritical(status);

    TaskSched();
}


void SoftDelay(void)
{
    char idx;
    for(idx = 0; idx < 0xFF; idx ++)
    {

    }
}


