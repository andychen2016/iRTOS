#ifndef __EVENT_H__
#define __EVENT_H__

#include "lib.h"


typedef enum _EventType{
    EventTypeUnknow = 0,
}EventType;


typedef struct _Event{
    EventType event_type;
    List wait_list;
}Event;


void EventInit(Event* event, EventType type);


#endif


