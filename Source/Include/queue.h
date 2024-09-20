#ifndef RTOSDEV_QUEUE_H
#define RTOSDEV_QUEUE_H
#include "event.h"


struct _Queue_Attr{
	Event_Attr event;
	uint32_t read;
	uint32_t write;
	uint32_t q_msg_cnt;
	uint32_t max_msg_cnt;
	void** msgBuffer;
};
typedef struct _Queue_Attr		Queue_Attr;

void queue_Init(Queue_Attr* q, void** msgBuffer, uint32_t maxMsg);
Event_Error queue_Read(Queue_Attr* q, void** msg, uint32_t waitTime);
Event_Error queue_Read_Nowait(Queue_Attr* q, void** msg, uint32_t waitTime);
Event_Error queue_Write(Queue_Attr* q, void* msg);
void queue_Refresh(Queue_Attr* q);
uint32_t queue_Delete(Queue_Attr* q);

#endif // Header file for rtosdev_queue
