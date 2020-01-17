#ifndef __TINYOS_H__
#define __TINYOS_H__

#include <stdint.h>
#include "lib.h"
#include "config.h"
#include "task.h"
#include "scheduler.h"
#include "ARMCM3.h"

#include "msgbox.h"
#include "sem.h"
#include "event.h"

#include "memblock.h"
#include "flaggroup.h"

#include "mutex.h"
#include "timer.h"
#include "hooks.h"


//extern vars
extern uint32_t 	gIdleCount;
extern Task 		gTaskIdle;
extern TaskStack 	gTaskIdleStack[TASK_STACK_SIZE];



//switch
void 		TaskRunFisrt(void);

void 		TaskSwitch(void);

uint32_t 	TaskEnterCritical(void);

void 		TaskExitCritical(uint32_t primask);

__asm void 	PendSV_Handler(void);





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

void SystemTickInit(void);

void InitCpuUseageStat(void);

void CheckCpuUsage(void);

void CpuUsageSyncWithSysTick (void);

float CpuUsageGet(void);



//time
void TaskDelay(uint32_t delay_sys_tick);

void SoftDelay(void);



#endif	//end os.h
