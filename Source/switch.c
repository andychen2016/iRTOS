
#include "os.h"
#include "ARMCM3.h"

#define NVIC_INT_CTRL       0xE000ED04      // 中断控制及状态寄存器
#define NVIC_PENDSVSET      0x10000000      // 触发软件中断的值
#define NVIC_SYSPRI2        0xE000ED22      // 系统优先级寄存器
#define NVIC_PENDSV_PRI     0x000000FF      // 配置优先级

#define MEM32(addr)         *(volatile unsigned long *)(addr)

__asm void PendSV_Handler(void)
{
    IMPORT gCurrentTask
    IMPORT gNextTask

		MRS R0, PSP
    CBZ R0, PendSVHANDLER_NoSave
    STMDB R0!,{R4-R11}

    LDR R1, =gCurrentTask
    LDR R1, [R1]
    STR R0, [R1]

PendSVHANDLER_NoSave
    LDR R0, =gCurrentTask
    LDR R1, = gNextTask
    LDR R2, [R1]
    STR R2, [R0]       ;gCurrentTask

    LDR R0, [R2]
    LDMIA R0!, {R4-R11}

    MSR PSP, R0;
    ORR LR, LR, #0x04
    BX LR
}

void TaskRunFisrt(void)
{
    __set_PSP(0);
    MEM32(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
    MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}

void TaskSwitch(void)
{
    MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}


//临界区保护
uint32_t TaskEnterCritical(void)
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();        // CPSID I
    return primask;
}

void TaskExitCritical(uint32_t primask)
{
    __set_PRIMASK(primask);
}





