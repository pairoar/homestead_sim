#ifndef _HOMESTEAD_LOG_SPOOL_H_
#define _HOMESTEAD_LOG_SPOOL_H_

#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "Event.h"
#include "Queue.h"

#define	LOG_BUFFER_SIZE	2048
#define DELIMITER 	'\0'

typedef struct LogSpoolContextType {
    uint16_t 		    in;
    uint16_t 		    out;

    SemaphoreHandle_t   *mutex;
    uint8_t 		    *buffer;
} LogSpoolContextType;


extern QueueType *log_spool_get_object(void);
extern void spool_init(void);

#endif  /* _HOMESTEAD_LOG_SPOOL_H_ */