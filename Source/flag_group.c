#include "tasks.h"
#include "event.h"
#include "task_schedule.h"
#include "rtos_dev_config.h"
#include "flag_group.h"

void flaggroup_Init(Flaggroup_Attr* f, uint32_t bits){
	event_Init(&f->event, Event_FlagGroup);
	f->flagBits = bits;
}

static Event_Error flag_check_handler(Flaggroup_Attr* f, uint32_t reqNeeds, uint32_t* req_flag_ptr){
	uint32_t req_flag = *req_flag_ptr;
	uint32_t bits_rm_check = FLAGGROUP_RM_BIT & reqNeeds;
	uint32_t bits_1_check = FLAGGROUP_SET & reqNeeds;
	uint32_t bits_all_check = FLAGGROUP_ALL & reqNeeds;
	
	uint32_t flag_aft_cmp = bits_1_check ? (f->flagBits & req_flag) : (~f->flagBits & req_flag);
	
	if(((bits_all_check != 0) && (req_flag == flag_aft_cmp)) || ((bits_all_check == 0) && (flag_aft_cmp != 0))){
		if(bits_rm_check){
			f->flagBits = (bits_1_check) ? (f->flagBits & (~flag_aft_cmp)) : (f->flagBits | flag_aft_cmp);
		} else {
			/** do nothing **/
		}
		*req_flag_ptr = flag_aft_cmp;
		return event_NoError;
	}
		*req_flag_ptr = flag_aft_cmp;
		return event_Unavaliable_Resource;
}

Event_Error flag_group_Wait(Flaggroup_Attr* f, uint32_t waitType, uint32_t reqflag, uint32_t* resflag, uint32_t waitTime){
	Event_Error result;
	uint32_t flags = reqflag;
	uint32_t status = tTaskEnterCritical();
	
	result = flag_check_handler(f, waitType, &flags);
	if (result != event_NoError){
		running_Task->reqflag_Type = waitType;
		running_Task->event_req_flag = reqflag;
		event_Wait_Block(&f->event, running_Task, (void*)0, Event_FlagGroup | RTOSDEV_TASK_STATE_WAIT_EVENT, waitTime);
		
		tTaskExitCritical(status);
		
		taskSchedule();
		
		*resflag = running_Task->event_req_flag;
		result = (Event_Error)running_Task->waitRes;
	} else {
		*resflag = flags;
		tTaskExitCritical(status);
	}
	return result;
}

void tFlagGroupNotify (Flaggroup_Attr* f, uint8_t isSet, uint32_t flag){
	tList* waitList;
	tNode* node;
	tNode* nextNode;
	uint8_t sched = 0;
	uint32_t status = tTaskEnterCritical();
	
	if (isSet){
		f->flagBits |= flag;
	} else {
		f->flagBits &= ~flag;
	}
	
	waitList = &f->event.waitList;
	for (node = waitList->headNode.nextNode; node != &(waitList->headNode); node = nextNode){
		uint32_t result;
		task_Attr_t* task = tNodeParent(node, task_Attr_t, linkNode);
		uint32_t flags = task->event_req_flag;
		nextNode = node->nextNode;
		
		result = flag_check_handler(f, task->reqflag_Type, &flags);
		if (result == event_NoError){
			task->event_req_flag = flags;
			event_Wakeup_Reqtask_Unblock(&f->event, task, (void *)0, event_NoError);
			sched = 1;
		}
	}
	
	if (sched){
		taskSchedule();
	}
	
	tTaskExitCritical(status);
}

uint32_t flaggroup_Delete(Flaggroup_Attr* f){
	uint32_t status = tTaskEnterCritical();
	uint32_t count = event_Delete(&f->event, (void *)0, event_Deleted);
	tTaskExitCritical(status);
	
	if (count > 0){
		taskSchedule();
	}
	return count;
}
