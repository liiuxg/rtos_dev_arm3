#ifndef RTOSDEV_TASK_SCHEDULE_H
#define RTOSDEV_TASK_SCHEDULE_H

void taskSchedule(void);
void task_SwitchStart(void);
void taskSwitch(void);
uint32_t tTaskEnterCritical (void);
void tTaskExitCritical (uint32_t status);

#endif // Header file for rtosdev_task_schedule
