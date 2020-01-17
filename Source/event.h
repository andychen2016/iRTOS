#ifndef __EVENT_H__
#define __EVENT_H__

#include "lib.h"
#include "task.h"


typedef enum _tEventType{
    EventTypeUnknow 	= 0,
    EventTypeSem 		= 1,
    EventTypeMBox 		= 2,
    EventTypeMemBlock 	= 3,
    EventTypeFlagGroup 	= 4,
    EventTypeMutex 		= 5
}	EventType;


typedef struct _Event{
    EventType 	eventType;
    List 		waitList;
}   Event;


void EventInit(Event* event, EventType type);

void EventWait(Event* event, Task* task, void* msg, uint32_t state, uint32_t timeout);

Task* EventWakeUp(Event* event, void* msg, uint32_t result);

void EventWakeUpTask(Event* event, Task* task, void* msg, uint32_t result);

void EventRemoveTask(Task* task, void* msg, uint32_t result);

uint32_t EventRemoveAll(Event* event, void* msg, uint32_t reseult);

uint32_t EventWaitCount(Event* event);

#endif


