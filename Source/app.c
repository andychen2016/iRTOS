#include "irtos.h"
#include "config.h"
#include "scheduler.h"
#include "event.h"
#include "sem.h"
#include "mutex.h"


Task gTask1, gTask2, gTask3, gTask4;
TaskStack gTask1_Stack[TASK_STACK_SIZE];
TaskStack gTask2_Stack[TASK_STACK_SIZE];
TaskStack gTask3_Stack[TASK_STACK_SIZE];
TaskStack gTask4_Stack[TASK_STACK_SIZE];

Task gTaskIdle;
TaskStack gTaskIdle_Stack[TASK_STACK_SIZE];

int gTask1_Flag = 0;
int gTask2_Flag = 0;
int gTask3_Flag = 0;
int gTask4_Flag = 0;

//Event
Event gEventWaitTimeout;
Event gEventWaitNormal;

Sem gSem1;
Sem gSem2;

MsgBox gMsgBox1;
MsgBox gMsgBox2;

void* gMBox1_MsgBuffer[20];
void* gMBox2_MsgBuffer[20];

uint32_t gMsg[20];

FlagGroup gFlagGroup;

Mutex gMutex;

Timer timer1;
Timer timer2;
Timer timer3;
uint32_t bit1 = 0;
uint32_t bit2 = 0;
uint32_t bit3 = 0;


void task1_func(void* param)
{
    unsigned long val = (unsigned long)param;
		
		float usage = 0.0;

    for(;;)
    {
				gTask1_Flag = 0;
        TaskDelay(1);
        gTask1_Flag = 1;
        TaskDelay(1);
				
				usage = CpuUsageGet();

    }

}

void task2_func(void* param)
{
    unsigned long val = (unsigned long)param;

    for(;;)
    {
				gTask2_Flag = 1;
        TaskDelay(1);
        gTask2_Flag = 0;
        TaskDelay(1);

    }

}

void task3_func(void* param)
{
    unsigned long val = (unsigned long)param;

    for(;;)
    {
				gTask3_Flag = 0;
        TaskDelay(1);
        gTask3_Flag = 1;
        TaskDelay(1);
    }
}

void task4_func(void* param)
{
    unsigned long val = (unsigned long)param;
    
    for(;;)
    {
        gTask4_Flag = 0;
        TaskDelay(1);
        gTask4_Flag =1;
        TaskDelay(1);
    }
}

void task_idle_func(void* param)
{
    SystemTickInit();
    InitCpuUseageStat();
    
    TaskSchedDisable();

    InitApps();
	
#if OS_ENABLE_TIMER == 1
    TimerInitTask();
#endif

    SetSysTickPeriod(OS_SYSTEM_TICK_MS);

    CpuUsageSyncWithSysTick();

    for(;;)
    {
        uint32_t  status = TaskEnterCritical();
        gIdleCount ++;
        TaskExitCritical(status);

    #if OS_ENABLE_HOOKS == 1
        CpuIdle_Hooks();
    #endif


	}
}


void InitApps(void)
{
    TaskInit(&gTask1, task1_func, 0x00, (void*)0x1111, gTask1_Stack, sizeof(gTask1_Stack));
    TaskInit(&gTask2, task2_func, 0x01, (void*)0x2222, gTask2_Stack, sizeof(gTask2_Stack));
    TaskInit(&gTask3, task3_func, 0x01, (void*)0x2222, gTask3_Stack, sizeof(gTask3_Stack));
    TaskInit(&gTask4, task4_func, 0x01, (void*)0x2222, gTask4_Stack, sizeof(gTask4_Stack));
}

