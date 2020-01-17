#ifndef __MEMBLOCK_H__
#define __MEMBLOCK_H__

#include "event.h"
#include "lib.h"

typedef struct _tMemBlock
{
    Event 		event;
    void* 		memStartAddr;
    uint32_t 	blockSize;
    uint32_t 	maxBlockCnt;
    List 		blockList;
} MemBlock;

typedef struct _tMemBlockInfo
{
    uint32_t 	count;
    uint32_t 	blockSize;
    uint32_t 	maxBlockCnt;
    uint32_t 	taskCount;
} 	MemBlockInfo;


void 		MemBlockInit(MemBlock* mem_block, uint8_t* start_addr, uint32_t block_size, uint32_t max_block_cnt);

uint32_t 	MemBlockWait(MemBlock* mem_block, uint8_t** mem, uint32_t wait_ticks);

uint32_t 	MemBlockNoWaitGet(MemBlock* mem_block, uint8_t** mem);

uint32_t 	MemBlockNotify(MemBlock* mem_block, uint8_t *mem);

uint32_t 	MemBlockDestroy(MemBlock* mem_block);

void		MemBlockGetInfo(MemBlock* mem_block, MemBlockInfo* info);

#endif




