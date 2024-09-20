#include "ARMCM3.h"
#include "rtos_dev_config.h"
#include "tasks.h"
#include "task_schedule.h"

/************************ Register for PendSV interrupt ************************/
#define	NVIC_INT_CTRL				0xE000Ed04
#define	NVIC_PENDSVSET			0x10000000
#define	NVIC_SYSPRI2				0xE000ED22
#define	NVIC_PENDSV_PRI			0x000000FF

#define	MEM32(addr)				*(volatile unsigned long *)(addr)
#define	MEM8(addr)				*(volatile unsigned char *)(addr)

/************************ Variables of task schedule ************************/

/************************ Task Switch for ARMCM3	************************/
/* Brief: task switch start
 * Input: None
 * Output: None
 */
void task_SwitchStart(void)
{
	__set_PSP(0);
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}

/* Brief: Call PendSV to handle task switch
 * Input: None
 * Output: None
 */
void taskSwitch(void)
{
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}

/* Brief: save current task stack and pop next task stack
 * Input: None
 * Output: None
 */
__asm void PendSV_Handler (void)
{
	IMPORT running_Task
	IMPORT next_Task
	
	MRS R0, PSP
	CBZ R0, PendSVHander_popstack
	
	STMDB R0!, {R4-R11}
	
	LDR R1, =running_Task
	LDR R1, [R1]
	STR R0, [R1]
	
PendSVHander_popstack
	LDR R0, =running_Task
	LDR R1, =next_Task
	LDR R2, [R1]
	STR R2, [R0]
	
	LDR R0, [R2]
	LDMIA R0!, {R4-R11}
	
	MSR PSP, R0
	ORR LR, LR, #0x04				; pop to user stack
	BX LR										; stop the interrupt
}

/************************ Enable/Disable task schedule	************************/
/* Brief: close interrupt and save current state for task
 * Input: None
 * Output: current state mask
 */
uint32_t tTaskEnterCritical (void)
{
	uint32_t primask = __get_PRIMASK();
	__disable_irq();
	return primask;
}

/* Brief: set the last state of interrupt
 * Input: last state of interrupt
 * Output: None
 */
void tTaskExitCritical (uint32_t status)
{
	__set_PRIMASK(status);
}

/************************ Task schedule	************************/
/* Brief: task schedule to get the next run task
 * Input: None
 * Output: None
 */
void taskSchedule(void){
	task_Attr_t* highest_ready_task;
	
	uint32_t status = tTaskEnterCritical();
	
	highest_ready_task = nxtTask_get_highest();
	if (highest_ready_task != running_Task){
		next_Task = highest_ready_task;
		taskSwitch();
	}
		
	tTaskExitCritical(status);
}
