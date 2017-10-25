#include "os.h"


int main(void)
{
    TaskSchedInit();
    TaskDelayListInit();
    InitApps();
   
		gNextTask = GetHighestReady();
	
		SetSysTickPeriod(1);
    TaskRunFisrt();
		return 0;
}

