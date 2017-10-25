#include "event.h"


void EventInit(Event* event, EventType type)
{
    event->event_type = type;
    ListInit(&(event->wait_list));
}

