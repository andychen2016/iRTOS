#ifndef __TASK_H__
#define __TASK_H__

#include "stdint.h"


//表征任务的状态，分为4种状态
#define TASK_STATE_READY		(0)
#define TASK_STATE_DELETED		(1<<1)
#define TASK_STATE_DELAYED		(1<<2)
#define TASK_STATE_SUSPEND		(1<<3)

#define TASK_WAIT_MASK          (0xFF << 16)

//提前定义
struct _Event;

//32位CPU的任务堆栈为uint_32
typedef uint32_t TaskStack;


typedef enum _Error{
	ErrorNoError 			= 0,
	ErrorTimeout 			= 1,
	ErrorResouceUnavaliable = 2,
	ErrorDeleted 			= 3,
	ErrorResouceFull 		= 4,
	ErrorWrongOwner 		= 5
}Error;



//任务结构体
typedef struct _tTask{
	TaskStack* 	ptrStack;		
	TaskStack* 	stackBase;
	uint32_t 	stackSize;

	Node 		linkNode;		
	uint32_t 	delaySysTick;
	uint32_t 	prio;
	Node 		delayNode;
	uint32_t 	state;
	uint32_t 	slice;
	uint32_t 	suspendCount;

	void 		(*cleanFun)(void* param);
	void* 		cleanParam;
	uint8_t 	requestDeleteFlag;

	//事件控制
	struct _Event* waitEvent;
	void* 		eventMsg;
	uint32_t 	waitEventResult;

	uint32_t 	waitFlagType;
	uint32_t 	eventRequestFlags;
} Task;



typedef struct _tTaskInfo{
	uint32_t 	delaySysTick;
	uint32_t 	prio;
	uint32_t 	slice;
	uint32_t 	suspendCount;

	uint32_t 	stackSize;
	uint32_t 	stackFreeSize;
	
}TaskInfo;



//----------task
void TaskInit(Task* ptr_task, void (*entry)(void*), uint32_t prio, void* param, TaskStack* ptr_stack, uint32_t size);

void TaskSuspend(Task* task);

void TaskWakeUp(Task* task);
//任务删除相关
void TaskSetCleanFunc(Task* task, void(*clean)(void*), void* param);

void TaskForceDelete(Task* task);

void TaskRequestDelete(Task* task);

uint8_t TaskIsRequestDelete(Task* task);


void TaskGetInfo(Task* task, TaskInfo* info);


void TaskDeleteSelf(Task* task);





#endif

