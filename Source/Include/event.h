#ifndef RTOSDEV_EVENT_H
#define RTOSDEV_EVENT_H

struct _task_Attr_t;

enum _Event_Error{
	event_NoError = 0,
	event_Timeout = 1,
	event_Unavaliable_Resource = 2,
	event_Deleted = 3,
};
typedef enum _Event_Error			Event_Error;

enum _Event_Type{
	Event_Default,
	Event_Semaphore,
	Event_Queue,
	Event_FlagGroup,
};
typedef enum _Event_Type			Event_Type;

struct _Event_Attr{
	Event_Type type;
	tList waitList;
};
typedef struct _Event_Attr		Event_Attr;

void event_Init(Event_Attr* event, Event_Type type);
void event_Wait_Block(Event_Attr* event, task_Attr_t* task, void* msg, uint32_t state, uint32_t waitTime);
task_Attr_t* event_Wakeup_Unblock(Event_Attr* event, void * msg, uint32_t result);
task_Attr_t* event_Wakeup_Reqtask_Unblock(Event_Attr* event, task_Attr_t* task, void * msg, uint32_t result);
void event_Outwait_task_Unblock(task_Attr_t *task, void * msg, uint32_t result);
uint32_t event_Delete(Event_Attr* event, void * msg, uint32_t result);
uint32_t event_Waitlist_Cnt(Event_Attr* event);

#endif // Header file for rtosdev_event
