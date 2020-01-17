#include "irtos.h"


int main(void)
{
    TaskSchedInit();
	
    TaskDelayListInit();
	
#if OS_ENABLE_TIMER == 1
    TimerModuleInit();
#endif

    //IDLE Task
    TaskInit(&gTaskIdle, task_idle_func, 31, (void*)0x0000, gTaskIdleStack, sizeof(gTaskIdleStack));

	gNextTask = GetHighestReady();
	

    TaskRunFisrt();
	return 0;
}

