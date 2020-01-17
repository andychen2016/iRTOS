#ifndef __FLAG_GROUP_H__
#define __FLAG_GROUP_H__

#include "event.h"

typedef struct _tFlagGroup
{
    Event 		event;
    uint32_t 	flags;
} 	FlagGroup;

typedef struct _tFlagGroupInfo
{
    uint32_t 	flags;
    uint32_t 	waitCount;
} FlagGroupInfo;



#define	FLAGGROUP_CLEAR		    (0x0 << 0)
#define	FLAGGROUP_SET			(0x1 << 0)
#define	FLAGGROUP_ANY			(0x0 << 1)
#define	FLAGGROUP_ALL			(0x1 << 1)

#define FLAGGROUP_SET_ALL		(FLAGGROUP_SET 		| FLAGGROUP_ALL)
#define	FLAGGROUP_SET_ANY		(FLAGGROUP_SET 		| FLAGGROUP_ANY)
#define FLAGGROUP_CLEAR_ALL	    (FLAGGROUP_CLEAR 	| FLAGGROUP_ALL)
#define FLAGGROUP_CLEAR_ANY	    (FLAGGROUP_CLEAR 	| FLAGGROUP_ANY)

#define	FLAGGROUP_CONSUME		(0x1 << 7)      //标志响应后是否被还原


void 		FlagGroupInit(FlagGroup* group, uint32_t init_flags);


uint32_t 	FlagGroupCheckAndConsume(FlagGroup* group, uint32_t type, uint32_t* flags);

uint32_t 	FlagGroupWait (FlagGroup * flag_group, uint32_t wait_type, uint32_t request_flag, uint32_t * result_flag, uint32_t wait_ticks);


uint32_t 	FlagGroupNoWaitGet(FlagGroup* group, uint32_t wait_type, uint32_t request_flags, uint32_t * result_flags);

void 		FlagGroupNotify(FlagGroup* group, uint32_t is_set, uint32_t flags);

void 		FlagGroupGetInfo(FlagGroup* group, FlagGroupInfo* info);

uint32_t 	FlagGroupDestroy(FlagGroup* group);






#endif

