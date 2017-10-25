#ifndef __TINYOS_H__
#define __TINYOS_H__

#include <stdint.h>
#include "lib.h"
#include "config.h"
#include "event.h"
#include "task.h"
#include "scheduler.h"
#include "ARMCM3.h"


//-----defines
//task state



//typedefs

			

//externs



//switch
void TaskRunFisrt(void);
void TaskSwitch(void);
uint32_t TaskEnterCritical(void);
void TaskExitCritical(uint32_t primask);
__asm void PendSV_Handler(void);
void TaskRunFisrt(void);





///app
void task1_func(void* param);
void task2_func(void* param);
void task3_func(void* param);
void task_idle_func(void* param);
void InitApps(void);


//CPU
void SetSysTickPeriod(uint32_t ms);
void TaskSysTick_Handler(void);
void SysTick_Handler(void);



//time
void TaskDelay(int delaySysTick);
void SoftDelay(void);


#endif	//end os.h
