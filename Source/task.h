#ifndef __TASK_H__
#define __TASK_H__

#include "stdint.h"



#define TASK_STATE_READY		0
#define TASK_STATE_DELETED		(1<<1)
#define TASK_STATE_DELAYED		(1<<2)
#define TASK_STATE_SUSPEND		(1<<3)


struct _Event;

typedef uint32_t tTaskStack;


typedef enum _Error{
	ErrorNoError = 0,
}Error;

typedef struct _tTask{
	tTaskStack* pStack;					
	Node linkNode;
	uint32_t delaySysTick;
	uint32_t prio;
	Node delayNode;
	uint32_t state;
	uint32_t slice;
	uint32_t suspendCount;

	void (*clean_fun)(void* param);
	void* clean_param;
	uint8_t requestDeleteFlag;

	//事件控制
	struct _Event* wait_event;
	void* event_msg;
	uint32_t wait_event_result;
} tTask;

typedef struct _TaskInfo{
	uint32_t delaySysTick;
	uint32_t prio;
	uint32_t slice;
	uint32_t suspendCount;
}TaskInfo;



//----------task
void TaskInit(tTask* pTask, void (*entry)(void*), uint32_t prio, void* param, tTaskStack* pStack);
void TaskSuspend(tTask* task);
void TaskWakeUp(tTask* task);
//任务删除相关
void TaskSetCleanFunc(tTask* task, void(*clean)(void*), void* param);
void TaskForceDelete(tTask* task);
void TaskRequestDelete(tTask* task);
uint8_t TaskIsRequestDelete(tTask* task);
void TaskGetInfo(tTask* task, TaskInfo* info);

void TaskDeleteSelf(tTask* task);





#endif

