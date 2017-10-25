#include "os.h"
#include "config.h"
#include "scheduler.h"

tTask gTask1, gTask2, gTask3, gTask4;
tTaskStack gTask1_Stack[TASK_STACK_SIZE];
tTaskStack gTask2_Stack[TASK_STACK_SIZE];
tTaskStack gTask3_Stack[TASK_STACK_SIZE];
tTaskStack gTask4_Stack[TASK_STACK_SIZE];

tTask gTaskIdle;
tTaskStack gTaskIdle_Stack[TASK_STACK_SIZE];

int gTask1_Flag = 0;
int gTask2_Flag = 0;
int gTask3_Flag = 0;
int gTask4_Flag = 0;

void Task1CleanFunc(void* param)
{
    gTask1_Flag = 0;
}

void Task3CleanFunc(void* param)
{
    gTask3_Flag = 0;
}

void task1_func(void* param)
{
    unsigned long val = (unsigned long)param;
    TaskSetCleanFunc(gCurrentTask,Task1CleanFunc, (void*)0);
    
    for(;;)
    {
        gTask1_Flag = 0;
        TaskDelay(5);
        gTask1_Flag = 1;
        TaskDelay(5);
    }

}

void task2_func(void* param)
{
    unsigned long val = (unsigned long)param;
    uint8_t task1deleted = 0;
    for(;;)
    {
        gTask2_Flag = 0;
        TaskDelay(5);
        gTask2_Flag = 1;
        TaskDelay(5);
        if(!task1deleted)
        {
            TaskForceDelete(&gTask1);
            task1deleted = 1;
        }
    }

}

void task3_func(void* param)
{
    unsigned long val = (unsigned long)param;

    for(;;)
    {
        if(TaskIsRequestDelete(gCurrentTask))
        {
            gTask3_Flag = 0;
            TaskDeleteSelf(gCurrentTask);
        }
        gTask3_Flag = 0;
				TaskDelay(5);
        gTask3_Flag =1;
        TaskDelay(5);
    }
}

void task4_func(void* param)
{
    unsigned long val = (unsigned long)param;
    uint8_t task3deleted = 0;
    for(;;)
    {
        gTask4_Flag = 0;
        TaskDelay(5);
        gTask4_Flag =1;
        TaskDelay(5);
        if(!task3deleted)
        {
            TaskRequestDelete(&gTask3);
            task3deleted = 1;
        }
    }
}

void task_idle_func(void* param)
{
		int idx;
    for(idx = 0; ; )
    {
			idx++;
		}
}


void InitApps(void)
{
    TaskInit(&gTask1, task1_func, 0x00, (void*)0x1111, &gTask1_Stack[TASK_STACK_SIZE]);
    TaskInit(&gTask2, task2_func, 0x01, (void*)0x2222, &gTask2_Stack[TASK_STACK_SIZE]);
    TaskInit(&gTask3, task3_func, 0x00, (void*)0x2222, &gTask3_Stack[TASK_STACK_SIZE]);
    TaskInit(&gTask4, task4_func, 0x01, (void*)0x2222, &gTask4_Stack[TASK_STACK_SIZE]);
    //IDLE Task
    TaskInit(&gTaskIdle, task_idle_func, 31, (void*)0x0000, &gTaskIdle_Stack[TASK_STACK_SIZE]);
}

