#ifndef __CONFIG_H__
#define __CONFIG_H__


#define TASK_SCHED_MAX_PRIO         32
#define TASK_SLICE_MAX              10
#define TASK_STACK_SIZE             1024

#define TASK_TIMER_MODULE_PRIO      1

#define OS_SYSTEM_TICK_MS           1

#define OS_PER_SECOND_TICKS         (1000 / OS_SYSTEM_TICK_MS)



//操作系统裁剪相关配置
#define OS_ENABLE_SEM               1
#define OS_ENABLE_FLAG_GROUP        1
#define OS_ENABLE_MEMBLOCK          1
#define OS_ENABLE_MSGBOX            1
#define OS_ENABLE_MUTEX             1
#define OS_ENABLE_TIMER             1
#define OS_ENABLE_HOOKS             1



#endif

