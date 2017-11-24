#include "irtos.h"


#if OS_ENABLE_MSGBOX == 1

void MsgBoxInit(MsgBox* msg_box, void** buffer, uint32_t max_count)
{
    EventInit(&(msg_box->event), EventTypeMBox);

    msg_box->count = 0;
    msg_box->max_count = max_count;
    msg_box->read_idx = 0;
    msg_box->write_idx = 0;
    msg_box->msg_buffer = buffer;
}



uint32_t MsgBoxWait(MsgBox* msg_box, void** msg, uint32_t wait_ticks)
{
    uint32_t status = TaskEnterCritical();

    if(msg_box->count > 0)
    {
        --msg_box->count;
        *msg = msg_box->msg_buffer[msg_box->read_idx ++];
        if(msg_box->read_idx >= msg_box->max_count)
        {
            msg_box->read_idx = 0;
        }
        TaskExitCritical(status);
        return ErrorNoError;
    }
    else
    {
        EventWait(&(msg_box->event), gCurrentTask, (void*)0, EventTypeMBox, wait_ticks);
        TaskExitCritical(status);

        TaskSched();
        
        *msg = gCurrentTask->event_msg;

        return gCurrentTask->wait_event_result;
    }

}


uint32_t MsgBoxNoWaitGet(MsgBox* msg_box, void **msg)
{
    uint32_t status =  TaskEnterCritical();

    if(msg_box->count > 0)
    {
        --msg_box->count;
        *msg = msg_box->msg_buffer[msg_box->read_idx ++];
        if(msg_box->read_idx >= msg_box->max_count)
        {
            msg_box->read_idx = 0;
        }
        TaskExitCritical(status);
        return ErrorNoError;
    }
    else
    {
        TaskExitCritical(status);
        return ErrorResouceUnavaliable;
    }

}


uint32_t MsgBoxNotify(MsgBox* msg_box, void* msg, uint32_t notify_opt)
{
    uint32_t status = TaskEnterCritical();

    if(EventWaitCount(&(msg_box->event)) > 0)
    {
        tTask* task = EventWakeUp(&(msg_box->event), msg, ErrorNoError);
        
        TaskExitCritical(status);

        if(task->prio < gCurrentTask->prio)
        {
            TaskSched();
        }
    }
    else
    {
        if(msg_box->count >= msg_box->max_count)
        {
            TaskExitCritical(status);
            return ErrorResouceFull;
        }
        
        if(notify_opt & MsgBoxSendFront)
        {
            if(msg_box->read_idx <= 0)
            {
                msg_box->read_idx = msg_box->max_count - 1;
            }
            else
            {
                --msg_box->read_idx;
            }
            msg_box->msg_buffer[msg_box->read_idx] = msg;
        }
        else
        {
            msg_box->msg_buffer[msg_box->write_idx++] = msg;
            if(msg_box->write_idx >= msg_box->max_count)
            {
                msg_box->write_idx = 0;
            }
        }
        
        msg_box->count++;

        TaskExitCritical(status);
    }

    return ErrorNoError;
}


void MsgBoxFlush(MsgBox* msg_box)
{
    uint32_t status = TaskEnterCritical();
    //如果任务队列中有任务，说明消息肯定是空的，就不需要清空
    if(EventWaitCount(&(msg_box->event)) == 0)
    {
        msg_box->count = 0;
        msg_box->read_idx = 0;
        msg_box->write_idx = 0;
    }

    TaskExitCritical(status);
}

uint32_t MsgBoxDestroy(MsgBox* msg_box)
{
    uint32_t status = TaskEnterCritical();

    uint32_t count = EventRemoveAll(&(msg_box->event), (void*)0, ErrorDeleted);

    TaskExitCritical(status);

    if(count > 0)
    {
        TaskSched();
    }

    return count;
}


void MsgBoxGetInfo(MsgBox* msg_box, MsgBoxInfo* info)
{
    uint32_t status = TaskEnterCritical();

    info->count = msg_box->count;
    info->max_count = msg_box->max_count;
    info->wait_task_num = EventWaitCount(&(msg_box->event));

    TaskExitCritical(status);
}


#endif
