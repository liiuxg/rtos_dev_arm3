#include "tasks.h"
#include "event.h"
#include "rtos_dev_config.h"
#include "task_schedule.h"
#include "ARMCM3.h"

/* Brief: timer for schedule
 * Input: interrupt time ms
 * Output: None
 */
void tSetSysTickPeriod(uint32_t ms){
	SysTick->LOAD = ms * SystemCoreClock / 1000 - 1;
	NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
					SysTick_CTRL_TICKINT_Msk |
					SysTick_CTRL_ENABLE_Msk;
}

static void Systimerhandler(void){
	tNode* node;
	uint32_t status = tTaskEnterCritical();

	for (node = taskDelayList.headNode.nextNode; node != &(taskDelayList.headNode); node = node->nextNode)
	{
		task_Attr_t* task = tNodeParent(node, task_Attr_t, delayNode);
		if (--task->delayTicks == 0){
			if(task->taskEvent){
				event_Outwait_task_Unblock(task, (void *)0, event_Timeout);
			}
			
			task_Outdelaystate_Unblock(task);
			taskSchedule_Ready(task);
		}
	}
	
	if (--running_Task->task_runTime == 0){
		if (tListCount(&taskTable[running_Task->taskPriority]) > 0){
			tListRemove(&taskTable[running_Task->taskPriority], &(running_Task->linkNode));
			tListAddLast(&taskTable[running_Task->taskPriority], &(running_Task->linkNode));
			running_Task->task_runTime = RTOSDEV_RUNTIME_DURATION_TICKS;
		}
	}

	tTaskExitCritical(status);
	taskSchedule();
}

/* Brief: timer handler
 * Input: None
 * Output: None
 */
void SysTick_Handler(void)
{
	Systimerhandler();
}
