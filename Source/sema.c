#include "tasks.h"
#include "sema.h"
#include "event.h"
#include "task_schedule.h"

void sem_Init(Sem_Attr* sema, uint32_t initCnt, uint32_t maxCnt){
	event_Init(&sema->event, Event_Semaphore);
	sema->max_cnt_number = maxCnt;	
	if(maxCnt){
		sema->cnt_number = (initCnt > maxCnt) ? maxCnt : initCnt;			
	} else { /** no limit for the sema count **/
		sema->cnt_number = initCnt;
	}
}

/* Brief: start a Semaphore to wait for a resource
 * Input: 
 * Output: 
 */
Event_Error sem_Wait_Time(Sem_Attr* sema, uint32_t waitTime){
	uint32_t status = tTaskEnterCritical();

	if (sema->cnt_number > 0){
		--sema->cnt_number;
		tTaskExitCritical(status);
		return event_NoError;
	} else {
		event_Wait_Block(&sema->event, running_Task, (void*)0, Event_Semaphore | RTOSDEV_TASK_STATE_WAIT_EVENT, waitTime);
		tTaskExitCritical(status);
		taskSchedule();
		
		return (Event_Error)running_Task->waitRes;
	}
}

Event_Error sem_Wait_Instant(Sem_Attr* sema){
		uint32_t status = tTaskEnterCritical();

		if (sema->cnt_number > 0){
			--sema->cnt_number;
			tTaskExitCritical(status);
			return event_NoError;
		} else {
			tTaskExitCritical(status);
			return event_Unavaliable_Resource;
		}
}

/* Brief: notify the tasks that are waiting for the resources
 * Input: 
 * Output: 
 */
void sem_Notify(Sem_Attr* sema){
	uint32_t status = tTaskEnterCritical();
	
	if(event_Waitlist_Cnt(&sema->event) > 0){
		task_Attr_t* task = event_Wakeup_Unblock(&sema->event, (void *)0, event_NoError);
		if(task->taskPriority < running_Task->taskPriority){
			taskSchedule();
		} else {
			/** do nothing **/
		}
	} else {
		++sema->cnt_number;
		if ((sema->max_cnt_number != 0) && (sema->cnt_number > sema->max_cnt_number)){
			sema->cnt_number = sema->max_cnt_number;
		}
	}
	tTaskExitCritical(status);
}

uint32_t sem_Release_All(Sem_Attr* sema){
	uint32_t status = tTaskEnterCritical();
	uint32_t count = event_Delete(&sema->event, (void *)0, event_Deleted);
	sema->cnt_number = 0;
	tTaskExitCritical(status);
	
	if(count > 0){
		taskSchedule();
	}
	
	return count;
}
