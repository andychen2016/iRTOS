#ifndef __MEMBLOCK_H__
#define __MEMBLOCK_H__

#include "event.h"
#include "lib.h"

typedef struct _tMemBlock
{
    Event event;
    void* mem_start_addr;
    uint32_t block_size;
    uint32_t max_block_cnt;
    List block_list;
} MemBlock;

typedef struct _tMemBlockInfo
{
    uint32_t count;
    uint32_t block_size;
    uint32_t max_block_cnt;
    uint32_t task_count;
} MemBlockInfo;


void MemBlockInit(MemBlock* mem_block, uint8_t* start_addr, uint32_t blocksize, uint32_t max_block_cnt);

uint32_t MemBlockWait(MemBlock* mem_block, uint8_t** mem, uint32_t wait_ticks);

uint32_t MemBlockNoWaitGet(MemBlock* mem_block, uint8_t** mem);

uint32_t MemBlockNotify(MemBlock* mem_block, uint8_t *mem);

uint32_t MemBlockDestroy(MemBlock* mem_block);

uint32_t MemBlockGetInfo(MemBlock* mem_block, MemBlockInfo* info);

#endif




