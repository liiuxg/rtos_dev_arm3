#include "tasks.h"
#include "event.h"
#include "task_schedule.h"
#include "sysinit.h"
#include "systimer.h"
#include "sema.h"
#include "queue.h"

task_Attr_t task1;
task_Attr_t task2;
task_Attr_t task3;
task_Attr_t task4;

taskBuffer_Size task1_taskBuffer[RTOSDEV_STACK_SIZE];
taskBuffer_Size task2_taskBuffer[RTOSDEV_STACK_SIZE];
taskBuffer_Size task3_taskBuffer[RTOSDEV_STACK_SIZE];
taskBuffer_Size task4_taskBuffer[RTOSDEV_STACK_SIZE];

int task1Flag;
void task1Entry(void * param){
	for (;;){			
		task1Flag = 0;
		rtosdev_delay(rtosdev_ms2ticks(1));
		task1Flag = 1;
		rtosdev_delay(rtosdev_ms2ticks(1));
	}
}

int task2Flag;
void task2Entry(void * param){	
	for (;;){			
		task2Flag = 0;
		rtosdev_delay(rtosdev_ms2ticks(1));
		task2Flag = 1;
		rtosdev_delay(rtosdev_ms2ticks(1));	
	}
}

int task3Flag;
void task3Entry(void * param){		
	for (;;){		
		task3Flag = 0;
		rtosdev_delay(rtosdev_ms2ticks(1));
		task3Flag = 1;
		rtosdev_delay(rtosdev_ms2ticks(1));
	}
}

int task4Flag;
void task4Entry(void * param){	
	for (;;){		
		task4Flag = 0;
		rtosdev_delay(rtosdev_ms2ticks(1));
		task4Flag = 1;
		rtosdev_delay(rtosdev_ms2ticks(1));
	}
}

int main(int argc, char** argv){
	rotsdev_sysinit();
	
	taskCreat(&task1, task1Entry, (void *)0, 0, &task1_taskBuffer[RTOSDEV_STACK_SIZE]);
	taskCreat(&task2, task2Entry, (void *)0, 1, &task2_taskBuffer[RTOSDEV_STACK_SIZE]);
	taskCreat(&task3, task3Entry, (void *)0, 1, &task3_taskBuffer[RTOSDEV_STACK_SIZE]);
	taskCreat(&task4, task4Entry, (void *)0, 1, &task4_taskBuffer[RTOSDEV_STACK_SIZE]);
	
	// start running rtos system
	rtosdev_sysstart();

	while(1){
		
	}
	// never reach here
}
