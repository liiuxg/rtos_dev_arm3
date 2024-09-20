#include "tasks.h"
#include "rtos_lib.h"
#include "task_schedule.h"
#include "event.h"

void event_Init(Event_Attr* event, Event_Type type){
	event->type = type;
	tListInit(&event->waitList);
}

#ifdef	PREEMPTION
/* Brief: In order to make the wait list from highest priority to lowest priority
 * Input: list, node
 * Output: None
 */
static void event_Priority_Inseart(tList* list, tNode* node){
	tNode* cur_node;
	task_Attr_t* cur_task;
	task_Attr_t* inseart_task = tNodeParent(node, task_Attr_t, linkNode);
	uint32_t priority = inseart_task->taskPriority;
	
	if(list->nodeCount == 0){
		tListAddLast(list, node);
		return;
	}
		
	for(cur_node = list->headNode.nextNode; cur_node != &(list->headNode); cur_node = cur_node->nextNode){
		cur_task = tNodeParent(cur_node, task_Attr_t, linkNode);
		if(cur_task->taskPriority > priority){
			tListInsertAfter(list, cur_node->preNode, node);
			return;
		}
	}		
	tListAddLast(list, node);
}
#endif

void event_Wait_Block(Event_Attr* event, task_Attr_t* task, void* msg, uint32_t state, uint32_t waitTime){
	
	uint32_t status = tTaskEnterCritical();
	
	task->taskState |= state;
	task->taskEvent = event;
	task->eventMsg = msg;
	task->waitRes = event_NoError;
	
	taskSchedule_Unready(task);
#ifdef	PREEMPTION
	event_Priority_Inseart(&event->waitList, &task->linkNode);				/** add to the event waiting list **/
#else
	tListAddLast(&event->waitList, &task->linkNode);
#endif
	
	if(waitTime){
		task_Indelaystate_Block(task, waitTime);
	}
	
	tTaskExitCritical(status);
}

/* Brief: wake up the highest priority task
 * Input: 
 * Output: 
 */
task_Attr_t* event_Wakeup_Unblock(Event_Attr* event, void * msg, uint32_t result){
	tNode * node;
	task_Attr_t * task = (task_Attr_t*)0;
	
	uint32_t status = tTaskEnterCritical();
	
	if((node = tListRemoveFirst(&event->waitList)) != (tNode *)0){
		task = (task_Attr_t *)tNodeParent(node, task_Attr_t, linkNode);
		task->taskEvent = (Event_Attr *)0;
		task->eventMsg = msg;
		task->waitRes = result;
		task->taskState &= ~RTOSDEV_TASK_STATE_WAIT_EVENT;
		
		if (task->delayTicks != 0){
			task_Outdelaystate_Unblock(task);
		}
		
		taskSchedule_Ready(task);
	}
	
	tTaskExitCritical(status);
	
	return task;
}

task_Attr_t* event_Wakeup_Reqtask_Unblock(Event_Attr* event, task_Attr_t* task, void * msg, uint32_t result){
	uint32_t status = tTaskEnterCritical();
	
	tListRemove(&event->waitList, &task->linkNode);
	task->taskEvent = (Event_Attr*)0;
	task->eventMsg = msg;
	task->waitRes = result;
	task->taskState &= ~RTOSDEV_TASK_STATE_WAIT_EVENT;
		
	if (task->delayTicks != 0){
		task_Outdelaystate_Unblock(task);
	}
		
	taskSchedule_Ready(task);
	
	tTaskExitCritical(status);
	return task;
}

/* Brief: release the required task from the wait list of a event
 * Input: 
 * Output: 
 */
void event_Outwait_task_Unblock(task_Attr_t *task, void * msg, uint32_t result){
	uint32_t status = tTaskEnterCritical();
	
	tListRemove(&task->taskEvent->waitList, &task->linkNode);
	task->taskEvent = (Event_Attr *)0;
	task->eventMsg = msg;
	task->waitRes = result;
	task->taskState &= ~RTOSDEV_TASK_STATE_WAIT_EVENT;

	tTaskExitCritical(status);
}

/* Brief: delete the event
 * Input: 
 * Output: 
 */
uint32_t event_Delete(Event_Attr* event, void * msg, uint32_t result){
	tNode * node;
	uint32_t count = 0;
	
	uint32_t status = tTaskEnterCritical();
	
	count = tListCount(&event->waitList);
	
	while((node = tListRemoveFirst(&event->waitList)) != (tNode *)0){
		task_Attr_t* task = (task_Attr_t *)tNodeParent(node, task_Attr_t, linkNode);
		task->taskEvent = (Event_Attr*)0;
		task->eventMsg = msg;
		task->waitRes = result;
		task->taskState &= ~RTOSDEV_TASK_STATE_WAIT_EVENT;
		
		if (task->delayTicks != 0){
			task_Outdelaystate_Unblock(task);
		}
		
		taskSchedule_Ready(task);
	}
	
	tTaskExitCritical(status);
	
	return count;
}

/* Brief: return how many tasks are in the event wait list
 * Input: 
 * Output: 
 */
uint32_t event_Waitlist_Cnt(Event_Attr* event){
	uint32_t count = 0;
	
	uint32_t status = tTaskEnterCritical();
	count = tListCount(&event->waitList);
	tTaskExitCritical(status);
	
	return count;
}
