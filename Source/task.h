#ifndef __TASK_H__
#define __TASK_H__

#include "stdint.h"



#define TASK_STATE_READY		0
#define TASK_STATE_DELETED		(1<<1)
#define TASK_STATE_DELAYED		(1<<2)
#define TASK_STATE_SUSPEND		(1<<3)

#define TASK_WAIT_MASK          (0xFF << 16)


struct _Event;

typedef uint32_t TaskStack;


typedef enum _Error{
	ErrorNoError = 0,
	ErrorTimeout = 1,
	ErrorResouceUnavaliable = 2,
	ErrorDeleted = 3,
	ErrorResouceFull = 4,
	ErrorWrongOwner =5,
}Error;

typedef struct _tTask{
	TaskStack* pStack;		
	uint32_t* stack_base;
	uint32_t stack_size;

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

	uint32_t wait_flag_type;
	uint32_t event_request_flags;
} Task;

typedef struct _TaskInfo{
	uint32_t delaySysTick;
	uint32_t prio;
	uint32_t slice;
	uint32_t suspendCount;

	uint32_t stack_size;
	uint32_t stack_free_size;
	
}TaskInfo;



//----------task
void TaskInit(Task* pTask, void (*entry)(void*), uint32_t prio, void* param, TaskStack* pStack, uint32_t size);
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

