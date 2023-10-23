#ifndef _HOMESTEAD_LOG_TASK_H_
#define _HOMESTEAD_LOG_TASK_H_

#include <stdint.h>

#include "Message.h"
#include "TaskInfo.h"
#include "Log.h"

typedef struct LogTaskType {
    TaskInfoType			*task;
    LogType					*log;
} LogTaskType;


extern void create_log_task(void);

#endif  /* _HOMESTEAD_LOG_TASK_H_ */