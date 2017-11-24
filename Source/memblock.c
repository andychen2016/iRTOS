#include "irtos.h"

#if OS_ENABLE_MEMBLOCK == 1

void MemBlockInit(MemBlock* mem_block, uint8_t* start_addr, uint32_t blocksize, uint32_t max_block_cnt)
{
    uint8_t* memBlockStart = (uint8_t*)start_addr;
    uint8_t* memBlockEnd = (uint8_t*)(start_addr + blocksize * max_block_cnt);

    if(blocksize < sizeof(Node))
    {
        return;
    }

    EventInit(&(mem_block->event), EventTypeMemBlock);

    mem_block->mem_start_addr = (void*)start_addr;
    mem_block->block_size = blocksize;
    mem_block->max_block_cnt = max_block_cnt;

    //初始化各个块的起始节点，然后加入到链表中
    ListInit(&(mem_block->block_list));
    while(memBlockStart < memBlockEnd)
    {
        NodeInit((Node*)memBlockStart);
        ListAddLast(&(mem_block->block_list), (Node*)memBlockStart);
        memBlockStart += blocksize;
    }

}

uint32_t MemBlockWait(MemBlock* mem_block, uint8_t** mem, uint32_t wait_ticks)
{
    uint32_t status = TaskEnterCritical();

    if(ListNodeCount(&(mem_block->block_list)) > 0)
    {
        *mem = (uint8_t*)ListRemoveFirst(&(mem_block->block_list));
        TaskExitCritical(status);
        return ErrorNoError;
    }
    else
    {
        EventWait(&(mem_block->event), gCurrentTask, (void*)0, EventTypeMemBlock, wait_ticks);
        TaskExitCritical(status);
        *mem = gCurrentTask->event_msg;

        return gCurrentTask->wait_event_result;
    }
}

uint32_t MemBlockNoWaitGet(MemBlock* mem_block, uint8_t** mem)
{
    uint32_t status = TaskEnterCritical();

    if(ListNodeCount(&(mem_block->block_list)) > 0)
    {
        *mem = (uint8_t*)ListRemoveFirst(&(mem_block->block_list));
        TaskExitCritical(status);
        return ErrorNoError;
    }
    else
    {
        TaskExitCritical(status);
        return ErrorResouceUnavaliable;
    }
    
}

uint32_t MemBlockNotify(MemBlock* mem_block, uint8_t *mem)
{
    uint32_t status = TaskEnterCritical();

    if(EventWaitCount(&(mem_block->event)) > 0)
    {
        tTask* task = EventWakeUp(&(mem_block->event), (void*)mem, ErrorNoError);
        TaskExitCritical(status);

        if(task->prio < gCurrentTask->prio)
        {
            TaskSched();
        }
        return ErrorNoError;
    }
    else
    {
        ListAddLast(&(mem_block->block_list), (Node*)mem);
        TaskExitCritical(status);
        
        return ErrorNoError;
    }

}

uint32_t MemBlockDestroy(MemBlock* mem_block)
{
    uint32_t status = TaskEnterCritical();

    uint32_t count = EventRemoveAll(&(mem_block->event), (void*)0, ErrorDeleted);

    TaskExitCritical(status);

    if(count > 0)
    {
        TaskSched();
    }

    return count;
}

uint32_t MemBlockGetInfo(MemBlock* mem_block, MemBlockInfo* info)
{
    uint32_t status = TaskEnterCritical();

    info->count = ListNodeCount(&(mem_block->block_list));
    info->block_size = mem_block->block_size;
    info->max_block_cnt = mem_block->max_block_cnt;
    info->task_count = EventWaitCount(&(mem_block->event));

    TaskExitCritical(status);
}

#endif