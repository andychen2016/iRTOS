#ifndef __TIMER_H__
#define __TIMER_H__

#include "lib.h"
#include "event.h"

#define TIMER_TYPE_HARD              (1 << 0)
#define TIMER_TYPE_SOFT              (0 << 0)

typedef enum _tTimerState
{
    TimerCreated = 0,
    TimerStarted = 1,
    TimerCallingFunc = 2,
    TimerStopped = 3,
    TimerClosed = 4,
} TimerState;

typedef struct _tTimer
{
    Node        link_node;
    uint32_t    start_delay_ticks;
    uint32_t    duration_ticks;
    uint32_t    decrease_ticks;                    //递减数值
    
    void        (*timer_func)(void* arg);       //回调函数
    void*       arg;                            //回调参数

    uint32_t    config;                         //配置为软件定时器或者是硬件定时器
    TimerState  state;
} Timer;

typedef struct _tTimerInfo
{
    uint32_t    start_delay_ticks;
    uint32_t    duration_ticks;

    void        (*timer_func)(void* arg);       //回调函数
    void*       arg;                            //回调参数
    uint32_t    config;                         //配置为软件定时器或者是硬件定时器
    TimerState  state;
} TimerInfo;


void TimerInit(Timer* timer, uint32_t delay_ticks, uint32_t duration_ticks, 
                void (*timerfunc)(void* arg), void* args, uint32_t config);



void TimerModuleInit(void);

void TimerStart(Timer* timer);
void TimerStop(Timer* timer);

void TimerClose(Timer* timer);

void TimerGetInfo(Timer* timer, TimerInfo* info);

void TimerCallFuncs(List* timer_list);

void TimerModuleTickNotify(void);

void TimerModuleSoftTask(void* param);

void TimerInitTask(void);

#endif



