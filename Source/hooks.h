#ifndef __HOOKS_H__
#define __HOOKS_H__

#include "irtos.h"

void CpuIdle_Hooks(void);

void SystemTick_Hooks(void);

void TaskSwitch_Hooks(Task* task_from, Task* task_to);

void TaskInit_Hooks(Task* task);



#endif


