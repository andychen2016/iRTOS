#include "irtos.h"

#if OS_ENABLE_MEMBLOCK == 1

void MemBlockInit(MemBlock* mem_block, uint8_t* start_addr, uint32_t block_size, uint32_t max_block_cnt)
{
    uint8_t* mem_block_start = (uint8_t*)start_addr;
    uint8_t* mem_block_end = (uint8_t*)(start_addr + block_size * max_block_cnt);

    if(block_size < sizeof(Node))
    {
        return;
    }

    EventInit(&(mem_block->event), EventTypeMemBlock);

    mem_block->memStartAddr = (void*)start_addr;
    mem_block->blockSize = block_size;
    mem_block->maxBlockCnt = max_block_cnt;

    //初始化各个块的起始节点，然后加入到链表中
    ListInit(&(mem_block->blockList));
    while(mem_block_start < mem_block_end)
    {
        NodeInit((Node*)mem_block_start);
        ListAddLast(&(mem_block->blockList), (Node*)mem_block_start);
        mem_block_start += block_size;
    }

}

uint32_t MemBlockWait(MemBlock* mem_block, uint8_t** mem, uint32_t wait_ticks)
{
    uint32_t status = TaskEnterCritical();

    if(ListNodeCount(&(mem_block->blockList)) > 0)
    {
        *mem = (uint8_t*)ListRemoveFirst(&(mem_block->blockList));
        TaskExitCritical(status);
        return ErrorNoError;
    }
    else
    {
        EventWait(&(mem_block->event), gCurrentTask, (void*)0, EventTypeMemBlock, wait_ticks);
        TaskExitCritical(status);
        *mem = gCurrentTask->eventMsg;

        return gCurrentTask->waitEventResult;
    }
}

uint32_t MemBlockNoWaitGet(MemBlock* mem_block, uint8_t** mem)
{
    uint32_t status = TaskEnterCritical();

    if(ListNodeCount(&(mem_block->blockList)) > 0)
    {
        *mem = (uint8_t*)ListRemoveFirst(&(mem_block->blockList));
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
        Task* task = EventWakeUp(&(mem_block->event), (void*)mem, ErrorNoError);
        TaskExitCritical(status);

        if(task->prio < gCurrentTask->prio)
        {
            TaskSched();
        }
        return ErrorNoError;
    }
    else
    {
        ListAddLast(&(mem_block->blockList), (Node*)mem);
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

void MemBlockGetInfo(MemBlock* mem_block, MemBlockInfo* info)
{
    uint32_t status = TaskEnterCritical();

    info->count = ListNodeCount(&(mem_block->blockList));
    info->blockSize = mem_block->blockSize;
    info->maxBlockCnt = mem_block->maxBlockCnt;
    info->taskCount = EventWaitCount(&(mem_block->event));

    TaskExitCritical(status);
}

#endif


