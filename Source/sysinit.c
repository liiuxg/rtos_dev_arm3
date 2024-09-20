#include "tasks.h"
#include "systimer.h"
#include "task_schedule.h"
#include "rtos_dev_config.h"

void rotsdev_sysinit(void){
	// init RTOSDEV_PRIORITY_LEVEL size ready lists 
	int index = 0;
	tBitmapInit(&task_Priority_Bitmap);
	for (index = 0; index < RTOSDEV_PRIORITY_LEVEL; index++){
		tListInit(&taskTable[index]);
	}	
	
	// init the delay list for delayed tasks
	tListInit(&taskDelayList);
	
	// init the relax function
	relax_Task_init();
}

void rtosdev_sysstart(void){
	next_Task = nxtTask_get_highest();
	tSetSysTickPeriod(RTOSDEV_INTERRUPT_TIME);	
	task_SwitchStart();	
}
