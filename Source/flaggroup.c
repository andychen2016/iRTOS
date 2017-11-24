#include "irtos.h"


#if OS_ENABLE_FLAG_GROUP == 1

void FlagGroupInit(FlagGroup* group, uint32_t init_flags)
{
    EventInit(&(group->event), EventTypeFlagGroup);
    group->flags = init_flags;
}


uint32_t FlagGroupCheckAndConsume(FlagGroup* group, uint32_t type, uint32_t* flags)
{
    uint32_t src_flags = *(flags);
    uint32_t is_set = type & (FLAGGROUP_SET);
    uint32_t is_all = type & (FLAGGROUP_ALL);
    uint32_t is_consume = type & (FLAGGROUP_CONSUME);

    uint32_t calc_flags = is_set ? (group->flags & src_flags) : (~group->flags & src_flags);

    if(((is_all != 0) && (calc_flags == src_flags)) || ((is_all == 0) && (calc_flags != 0)))
    {
        if(is_consume)
        {
            group->flags &= ~calc_flags;
        }
        else
        {
            group->flags |= calc_flags;
        }
        *flags = calc_flags;
        return ErrorNoError;
    }

    *flags = calc_flags;

    return ErrorResouceUnavaliable;
}



uint32_t FlagGroupWait (FlagGroup * flag_group, 
                        uint32_t wait_type, 
                        uint32_t request_flags,
                        uint32_t * result_flags, 
                        uint32_t wait_ticks)
{
    uint32_t result;
    uint32_t flags = request_flags;

    uint32_t status = TaskEnterCritical();

    result = FlagGroupCheckAndConsume(flag_group, wait_type, &flags);
    if(result != ErrorNoError)
    {
        gCurrentTask->wait_flag_type = wait_type;
        gCurrentTask->event_request_flags = request_flags;

        EventWait(&(flag_group->event), gCurrentTask, (void*)0, EventTypeFlagGroup, wait_ticks);

        TaskExitCritical(status);

        TaskSched();

        *result_flags = gCurrentTask->event_request_flags;
        result = gCurrentTask->wait_event_result;
    }
    else
    {
        *result_flags = flags;
        TaskExitCritical(status);
    }

    return result;
}

uint32_t FlagGroupNoWaitGet(FlagGroup* group, uint32_t wait_type, 
                            uint32_t request_flags, uint32_t * result_flags)
{
    uint32_t flags = request_flags;
    uint32_t status = TaskEnterCritical();

    uint32_t result = FlagGroupCheckAndConsume(group, wait_type, &flags);

    TaskExitCritical(status);

    return result;
}


void FlagGroupNotify(FlagGroup* group, uint32_t is_set, uint32_t flags)
{
    List *wait_list;
    Node *node;
    Node * next_node;
    uint8_t sched = 0;
    uint32_t result;

    uint32_t status = TaskEnterCritical();

    if(is_set)
    {
        group->flags |= flags;
    }
    else
    {
        group->flags &= ~flags;
    }

    wait_list = &(group->event.wait_list);
    for(node = wait_list->headNode.nextNode; node != &(wait_list->headNode); node = next_node)
	{
        tTask* task = ParentAddress(node, tTask, linkNode);
        uint32_t flags_inner = task->event_request_flags;

        next_node = node->nextNode;     //这里一定要提前取，不然后面EventWakeUpTask会将task->linkNode移除

        result = FlagGroupCheckAndConsume(group, task->wait_flag_type, &flags_inner);
        if(result == ErrorNoError)
        {
            task->event_request_flags = flags_inner;
            EventWakeUpTask(&(group->event), task, (void*)0, ErrorNoError);
            sched = 1;  
        } 
    }

    if(sched)
    {
        TaskSched();
    }

    TaskExitCritical(status);

}


void FlagGroupGetInfo(FlagGroup* group, FlagGroupInfo* info)
{
    uint32_t status = TaskEnterCritical();

    info->wait_count = EventWaitCount(&(group->event));
    info->flags = group->flags;
    
    TaskExitCritical(status);
}

uint32_t FlagGroupDestroy(FlagGroup* group)
{
    uint32_t status = TaskEnterCritical();

    uint32_t count = EventRemoveAll(&(group->event), (void*)0, ErrorDeleted);
    
    TaskExitCritical(status);

    return count;
}

#endif
