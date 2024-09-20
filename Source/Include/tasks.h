#ifndef RTOSDEV_TASKS_H
#define RTOSDEV_TASKS_H

#include <stdint.h>
#include "rtos_dev_config.h"
#include "rtos_lib.h"

/** task state define **/
#define	RTOSDEV_TASK_STATE_READY					 0
#define	RTOSDEV_TASK_STATE_DELETED				(1 << 9)
#define	RTOSDEV_TASK_STATE_DELAYED				(1 << 10)
#define	RTOSDEV_TASK_STATE_SUSPEND				(1 << 11)

#define	RTOSDEV_TASK_STATE_WAIT_EVENT			(0xFF << 16)

/** task delete flag **/
#define TASK_DELETE_DEFAULT								 0
#define TASK_DELETE_DELETE_FLAG						 1

struct _Event_Attr;
typedef uint32_t taskBuffer_Size;
typedef void (*task_del_cbk_t)(void*);

struct _task_Attr_t{
	taskBuffer_Size* task_buffer;
	
	uint32_t delayTicks;
	uint32_t taskPriority;
	uint32_t taskState;
	uint32_t task_runTime;
	uint32_t suspendCount;
	
	tNode linkNode;
	tNode delayNode;
	
	void (*taskDele_handler) (void * arg);
	void * taskDele_arg;
	uint8_t requestDeleteFlag;
	
	struct _Event_Attr* taskEvent;
	void* eventMsg;
	uint32_t waitRes;
	
	uint32_t reqflag_Type;
	uint32_t event_req_flag;
};
typedef struct _task_Attr_t		task_Attr_t;

extern tBitmap task_Priority_Bitmap;
extern tList taskTable[RTOSDEV_PRIORITY_LEVEL];
extern tList taskDelayList;

extern struct _task_Attr_t* running_Task;
extern struct _task_Attr_t* next_Task;
extern struct _task_Attr_t* relax_Task_ptr;

/** relax function headers **/
void relax_Task_init(void);

/** task function headers **/
task_Attr_t* nxtTask_get_highest(void);
void taskCreat(task_Attr_t* task, void (*entry)(void* ), void* arg, uint32_t priority, taskBuffer_Size* stack);

/** task ready and unready functions header **/
void taskSchedule_Ready(task_Attr_t* task);
void taskSchedule_Unready(task_Attr_t* task);

/** task delay functions header **/
void task_Outdelaystate_Unblock(task_Attr_t* task);
void task_Indelaystate_Block(task_Attr_t * task, uint32_t ticks);
uint32_t rtosdev_ms2ticks(uint32_t delay_time);
void rtosdev_delay(uint32_t delay_ticks);

/** task suspend functions header **/
void task_Outsuspend_Unblock(task_Attr_t* task);
void task_Insuspend_Block(task_Attr_t * task);

/** task delete functions header **/
void task_Delcbk_Set(task_Attr_t* task, task_del_cbk_t funk, void* arg);
void task_Delete_Instant(task_Attr_t* task);
void task_Dele_Request(task_Attr_t* task);
uint8_t task_Request_Check(void);
void task_OutRedylist_Delete(task_Attr_t* task);
void task_SelfDelete(void);

#endif // Header file for rtosdev_tasks
