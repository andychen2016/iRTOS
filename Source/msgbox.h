#ifndef __MSGBOX_H__
#define __MSGBOX_H__

#include "event.h"


#define MsgBoxSendDefault       0x00
#define MsgBoxSendFront         0x01


typedef struct _tMsgBox
{
    Event 		event;
    uint32_t 	count;
    uint32_t 	maxCount;
    uint32_t 	readIdx;
    uint32_t 	writeIdx;
    void** 		msgBuffer;
} MsgBox;

typedef struct _tMsgBoxInfo
{
    uint32_t count;
    uint32_t maxCount;
    uint32_t waitTaskNum;
} MsgBoxInfo;

void 		MsgBoxInit(MsgBox* msg_box, void** buffer, uint32_t max_count);

uint32_t 	MsgBoxWait(MsgBox* msg_box, void** msg, uint32_t wait_ticks);

uint32_t 	MsgBoxNoWaitGet(MsgBox* msg_box, void **msg);

uint32_t 	MsgBoxNotify(MsgBox* msg_box, void* msg, uint32_t notify_opt);


#endif



