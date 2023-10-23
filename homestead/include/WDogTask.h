#ifndef _HOMESTEAD_WDOG_TASK_H_
#define _HOMESTEAD_WDOG_TASK_H_

#include <stdint.h>

#include "Message.h"
#include "TaskInfo.h"
#include "WDog.h"

typedef struct WDogTaskType {
    TaskInfoType			*task;
    WDogType				*dog;
} WDogTaskType;


extern void create_wdog_task(void);

#endif  /* _HOMESTEAD_WDOG_TASK_H_ */