#ifndef RTOSDEV_SEMA_H
#define RTOSDEV_SEMA_H
#include "event.h"

struct _Sem_Attr{
	Event_Attr event;
	uint32_t cnt_number;
	uint32_t max_cnt_number;
};
typedef struct _Sem_Attr			Sem_Attr;

void sem_Init(Sem_Attr* sema, uint32_t initCnt, uint32_t maxCnt);
Event_Error sem_Wait_Time(Sem_Attr* sema, uint32_t waitTime);
Event_Error sem_Wait_Instant(Sem_Attr* sema);
void sem_Notify(Sem_Attr* sema);
uint32_t sem_Release_All(Sem_Attr* sema);

#endif // Header file for rtosdev_semaphore
