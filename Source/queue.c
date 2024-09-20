#include "tasks.h"
#include "queue.h"
#include "event.h"
#include "task_schedule.h"

void queue_Init(Queue_Attr* q, void** msgBuffer, uint32_t maxMsg){
	event_Init(&q->event, Event_Queue);
	q->max_msg_cnt = maxMsg;
	q->msgBuffer = msgBuffer;
	q->read = q->write = q->q_msg_cnt = 0;
}

Event_Error queue_Read(Queue_Attr* q, void** msg, uint32_t waitTime){
	uint32_t status = tTaskEnterCritical();
	
	if(q->q_msg_cnt > 0){
		--q->q_msg_cnt;
		*msg = q->msgBuffer[q->read++];
		if(q->read >= q->max_msg_cnt){
			q->read = 0;
		}
		tTaskExitCritical(status);
		return event_NoError;
	} else {
		event_Wait_Block(&q->event, running_Task, (void*)0, Event_Queue | RTOSDEV_TASK_STATE_WAIT_EVENT, waitTime);
		tTaskExitCritical(status);
		taskSchedule();
		
		*msg = running_Task->eventMsg;
		return (Event_Error)running_Task->waitRes;
	}
}

Event_Error queue_Read_Nowait(Queue_Attr* q, void** msg, uint32_t waitTime){
	uint32_t status = tTaskEnterCritical();	
	
	if(q->q_msg_cnt > 0){
		--q->q_msg_cnt;
		*msg = q->msgBuffer[q->read++];
		if(q->read >= q->max_msg_cnt){
			q->read = 0;
		}
		tTaskExitCritical(status);
		return event_NoError;
	} else {
		
		tTaskExitCritical(status);
		return event_Unavaliable_Resource;
	}
}

Event_Error queue_Write(Queue_Attr* q, void* msg){
	uint32_t status = tTaskEnterCritical();	
	
	if(event_Waitlist_Cnt(&q->event) > 0){ // write to the list when there are avaliable space
		
		task_Attr_t* task = event_Wakeup_Unblock(&q->event, (void*)msg, event_NoError);
		if(task->taskPriority < running_Task->taskPriority){
			taskSchedule();
		}
		
	} else {															// enough space to write msgs
		
		if(q->q_msg_cnt >= q->max_msg_cnt){
			tTaskExitCritical(status);
			return event_Unavaliable_Resource;
		} else {
			q->msgBuffer[q->write++] = msg;
			if(q->write >= q->max_msg_cnt){
				q->write = 0;
			}
		}
		
		q->q_msg_cnt++;
	}
	
	tTaskExitCritical(status);
	return event_NoError;
}

void queue_Refresh(Queue_Attr* q){
	uint32_t status = tTaskEnterCritical();
	
	if (event_Waitlist_Cnt(&q->event) == 0){
		q->read = 0;
		q->write = 0;
		q->q_msg_cnt = 0;
	}
	
	tTaskExitCritical(status);
}

uint32_t queue_Delete(Queue_Attr* q){
	uint32_t status = tTaskEnterCritical();
	uint32_t count = event_Delete(&q->event, (void *)0, event_Deleted);
	tTaskExitCritical(status);
	
	if (count > 0){
		taskSchedule();
	}
	return count;
}
