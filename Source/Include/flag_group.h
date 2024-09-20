#ifndef RTOSDEV_QUEUE_H
#define RTOSDEV_QUEUE_H
#include "event.h"

#define	FLAGGROUP_CLEAR		(0x0 << 0)
#define	FLAGGROUP_SET			(0x1 << 0)
#define	FLAGGROUP_ANY			(0x0 << 1)
#define	FLAGGROUP_ALL			(0x1 << 1)
#define FLAGGROUP_RM_BIT	(0x1 << 2)


#define	FLAGGROUP_SET_ALL		(TFLAGGROUP_SET | TFLAGGROUP_ALL)
#define	FLAGGROUP_SET_ANY		(TFLAGGROUP_SET | TFLAGGROUP_ANY)
#define	FLAGGROUP_CLEAR_ALL	(TFLAGGROUP_CLEAR | TFLAGGROUP_ALL)
#define	FLAGGROUP_CLEAR_ANY	(TFLAGGROUP_CLEAR | TFLAGGROUP_ANY)


struct _Flaggroup_Attr{
	Event_Attr event;
	uint32_t flagBits;
};
typedef struct _Flaggroup_Attr		Flaggroup_Attr;

void flaggroup_Init(Flaggroup_Attr* f, uint32_t bits);
Event_Error flag_group_Wait(Flaggroup_Attr* f, uint32_t waitType, uint32_t reqflag, uint32_t* resflag, uint32_t waitTime);
void tFlagGroupNotify (Flaggroup_Attr* f, uint8_t isSet, uint32_t flag);
uint32_t flaggroup_Delete(Flaggroup_Attr* f);

#endif // Header file for rtosdev_flag_group
