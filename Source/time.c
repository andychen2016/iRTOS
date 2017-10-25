#include "os.h"


void TaskDelay(int delaySysTick)
{
    uint32_t status = TaskEnterCritical();
	
    TimeTaskWait(gCurrentTask, delaySysTick);
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