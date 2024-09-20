#include "tasks.h"
#include "rtos_lib.h"
#include "rtos_dev_config.h"
#include "task_schedule.h"

tBitmap task_Priority_Bitmap;								// Resource manage to get the most important task
tList taskTable[RTOSDEV_PRIORITY_LEVEL];		// task table to store different priority tasks
tList taskDelayList;												// List to link the delayed list

struct _task_Attr_t* running_Task;
struct _task_Attr_t* next_Task;
struct _task_Attr_t* relax_Task_ptr;

/************************ Relax	function ************************/
struct _task_Attr_t relax_Task;
taskBuffer_Size relax_taskBuffer[RTOSDEV_STACK_SIZE];
void relax_Taskrun(void* arg){
	for (;;){
	}
}

void relax_Task_init(void){
		taskCreat(&relax_Task, relax_Taskrun, (void *)0, RTOSDEV_PRIORITY_LEVEL - 1, &relax_taskBuffer[RTOSDEV_STACK_SIZE]);
		relax_Task_ptr = &relax_Task;
}


/************************ Task functions	************************/
void taskCreat(task_Attr_t* task, void (*entry)(void* ), void* arg, uint32_t priority, taskBuffer_Size* stack){
	*(--stack) = (unsigned long)(1 << 24);
	*(--stack) = (unsigned long)entry;
	*(--stack) = (unsigned long)0x0;
	*(--stack) = (unsigned long)0x0;
	*(--stack) = (unsigned long)0x0;
	*(--stack) = (unsigned long)0x0;
	*(--stack) = (unsigned long)0x0;
	*(--stack) = (unsigned long)arg;
	
	*(--stack) = (unsigned long)0x0;
	*(--stack) = (unsigned long)0x0;
	*(--stack) = (unsigned long)0x0;
	*(--stack) = (unsigned long)0x0;
	*(--stack) = (unsigned long)0x0;
	*(--stack) = (unsigned long)0x0;
	*(--stack) = (unsigned long)0x0;
	*(--stack) = (unsigned long)0x0;
	
	task->task_buffer = stack;
	task->delayTicks = 0;
	task->taskPriority = priority;
	
	task->task_runTime = RTOSDEV_RUNTIME_DURATION_TICKS;
	task->taskState = RTOSDEV_TASK_STATE_READY;
	
	task->suspendCount = 0;
	task->taskDele_handler = (task_del_cbk_t)0;
	task->taskDele_arg = (void *)0;
	task->requestDeleteFlag = TASK_DELETE_DEFAULT;
	
	tNodeInit(&(task->delayNode));
	tNodeInit(&(task->linkNode));

	taskSchedule_Ready(task);
	
	//Event define 
	task->eventMsg = (void*)0;
	task->taskEvent = (struct _Event_Attr*)0;
	task->waitRes = 0;
	
	task->event_req_flag = 0;
	task->reqflag_Type = 0;
}

/* Brief: get the next task in the highest task linked list 
 * Input: None
 * Output: None
 */
task_Attr_t* nxtTask_get_highest(void){
	uint32_t ready_table_index = tBitmapGetFirstSet(&task_Priority_Bitmap);
	tNode* node = tListFirst(&taskTable[ready_table_index]);
	return tNodeParent(node, task_Attr_t, linkNode);
}

/* Brief: add the task to the ready task list
 * Input: task_Attr_t* task
 * Output: None
 */
void taskSchedule_Ready(task_Attr_t* task){
	tListAddFirst(&(taskTable[task->taskPriority]), &(task->linkNode));
	tBitmapSet(&task_Priority_Bitmap, task->taskPriority);
}

/* Brief: add the task to the ready task list
 * Input: task_Attr_t* task
 * Output: None
 */
void taskSchedule_Unready(task_Attr_t* task){
	tListRemove(&(taskTable[task->taskPriority]), &(task->linkNode));
	
	if (tListCount(&taskTable[task->taskPriority]) == 0){
		tBitmapClear(&task_Priority_Bitmap, task->taskPriority);
	}
}

/***************************************** Section for task delay ****************************************
* Functions: 1) void task_Outdelaystate_Unblock(task_Attr_t* task);
*					   2) void task_Indelaystate_Block(task_Attr_t * task, uint32_t ticks);
*						 3) uint32_t rtosdev_ms2ticks(uint32_t delay_time);
*						 4) void rtosdev_delay(uint32_t delay_ticks);
*/

/* Brief: remove the input task from the delayed list
 * Input: task_Attr_t* task
 * Output: None
 */
void task_Outdelaystate_Unblock(task_Attr_t* task){
	tListRemove(&taskDelayList, &(task->delayNode));
	task->taskState &= ~RTOSDEV_TASK_STATE_DELAYED;
}

/* Brief: insert the input task to the delayed list and block itself for ticks time
 * Input: task_Attr_t* task
 * Output: None
 */
void task_Indelaystate_Block(task_Attr_t * task, uint32_t ticks){
	task->delayTicks = ticks;
	tListAddLast(&taskDelayList, &(task->delayNode));
	task->taskState |= RTOSDEV_TASK_STATE_DELAYED;
}

/* Brief: convert the delay time from ms to ticks
 * Input: delay time ms
 * Output: None
 */
uint32_t rtosdev_ms2ticks(uint32_t delay_time){
	uint32_t ticks = delay_time / RTOSDEV_INTERRUPT_TIME;
	return ticks;
}

/* Brief: task delay for ticks time
 * Input: task_Attr_t* task
 * Output: None
 */
void rtosdev_delay(uint32_t delay_ticks){
	uint32_t status = tTaskEnterCritical();
	
	task_Indelaystate_Block(running_Task, delay_ticks);
	taskSchedule_Unready(running_Task);

	tTaskExitCritical(status);
	taskSchedule();
}

/***************************************** Section for task suspend ****************************************
* Functions: 1) void task_Outsuspend_Unblock(task_Attr_t* task);
*						 2) void task_Insuspend_Block(task_Attr_t * task);
*/

/* Brief: unblock task from suspend state
 * Input: task_Attr_t* task
 * Output: None
 */
void task_Outsuspend_Unblock(task_Attr_t* task){
	uint32_t status = tTaskEnterCritical();
	
	if(task->taskState & RTOSDEV_TASK_STATE_SUSPEND){
		if(--task->suspendCount == 0){
			task->taskState &= ~RTOSDEV_TASK_STATE_SUSPEND;
			taskSchedule_Ready(task);
			taskSchedule();
		}	
	} else {
		/* do nothing */
	}
	
	tTaskExitCritical(status);
}

/* Brief: block task in a suspend state
 * Input: task_Attr_t* task
 * Output: None
 */
void task_Insuspend_Block(task_Attr_t* task){
	uint32_t status = tTaskEnterCritical();
	
	if((task->taskState & RTOSDEV_TASK_STATE_DELAYED) || (task->taskState & RTOSDEV_TASK_STATE_DELETED)){
		return;
	}
	
	if(task->suspendCount++ == 0){
			task->taskState |= RTOSDEV_TASK_STATE_SUSPEND;
			taskSchedule_Unready(task);
			if (task == running_Task){
				taskSchedule();
			}
	} else {
		/* do nothing */
	}
		
	tTaskExitCritical(status);
}

/***************************************** Section for task delete ****************************************
* Functions: 1) void task_Delcbk_Set(task_Attr_t* task, task_del_cbk_t funk, void* arg);
*						 2) void task_delete_instant(task_Attr_t* task);
*						 3) void task_dele_request(task_Attr_t* task);
*						 4) uint8_t task_Request_Check(void);
*						 5) void task_SelfDelete(void);
* 					 6) void task_OutRedylist_Delete(task_Attr_t* task);
*/

/* Brief: rm task from ready list
 * Input: task_Attr_t* task
 * Output: None
 */
void task_OutRedylist_Delete(task_Attr_t* task){
	tListRemove(&(taskTable[task->taskPriority]), &(task->linkNode));
	
	if (tListCount(&taskTable[task->taskPriority]) == 0){
		tBitmapClear(&task_Priority_Bitmap, task->taskPriority);
	}
}

/* Brief: Assign task_del_cbk func and arg to task
 * Input: 
 * Output: None
 */
void task_Delcbk_Set(task_Attr_t* task, task_del_cbk_t func, void* arg){
	task->taskDele_handler = func;
	task->taskDele_arg = arg;
}

/* Brief: delete the input task instantly
 * Input: task_Attr_t* task
 * Output: None
 */
void task_Delete_Instant(task_Attr_t* task){
	uint32_t status = tTaskEnterCritical();
	
	if (task->taskState & RTOSDEV_TASK_STATE_DELAYED){
		task_Outdelaystate_Unblock(task);
	} else if (!(task->taskState & RTOSDEV_TASK_STATE_SUSPEND)){
		task_OutRedylist_Delete(task);
	}
	
	if(task->taskDele_handler){
		task->taskDele_handler(task->taskDele_arg);
	}
	
	if(running_Task == task){
		taskSchedule();
	}
	
	tTaskExitCritical(status);	
}

/* Brief: set delete request
 * Input: task_Attr_t* task
 * Output: None
 */
void task_Dele_Request(task_Attr_t* task){
	uint32_t status = tTaskEnterCritical();
	
	task->requestDeleteFlag = TASK_DELETE_DELETE_FLAG;
	
	tTaskExitCritical(status);
}

/* Brief: delete request result return
 * Input: 
 * Output: None
 */
uint8_t task_Request_Check(void)
{
	uint8_t dflag;
	uint32_t status = tTaskEnterCritical();
	
	dflag = running_Task->requestDeleteFlag;
	
	tTaskExitCritical(status);
	return dflag;
}

/* Brief: task start delete itself when receiving the delete request
 * Input: 
 * Output: None
 */
void task_SelfDelete(void){
	uint32_t status = tTaskEnterCritical();
	
	task_OutRedylist_Delete(running_Task);
	if (running_Task->taskDele_handler){
		running_Task->taskDele_handler(running_Task->taskDele_arg);
	}
	taskSchedule();
	
	tTaskExitCritical(status);
}
