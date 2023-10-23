#ifndef _HOMESTEAD_TASK_INFO_H_
#define _HOMESTEAD_TASK_INFO_H_

#include <stdint.h>

#include <FreeRTOS.h>
#include <queue.h>

#include "Message.h"


typedef enum {
	TASK_ID_MAIN = 0,
	TASK_ID_DOG,
	TASK_ID_LOG,
	TASK_ID_ATC,
	TASK_ID_PRODUCT,
	TASK_ID_KEY,
	TASK_ID_MAX,
} TaskIdType;

#define TASK_ID_MASK_MAIN		(1 << TASK_ID_MAIN)
#define TASK_ID_MASK_DOG		(1 << TASK_ID_DOG)
#define TASK_ID_MASK_LOG		(1 << TASK_ID_LOG)
#define TASK_ID_MASK_ATC		(1 << TASK_ID_ATC)
#define TASK_ID_MASK_PRODUCT	(1 << TASK_ID_PRODUCT)

typedef struct TaskContextType {
	TaskIdType			id;
    TaskHandle_t        task;
    const char          *name;
    int					priority;
    int					stackSize;
} TaskContextType;

typedef enum {
	PROVIDER_NONE,
	PROVIDER_DOG,
	PROVIDER_LOG,
	PROVIDER_KEY,
	PROVIDER_MAX,
} ProviderIdType;

typedef struct ProviderType {
	TaskIdType      taskId;
	ProviderIdType  type;
	uint32_t        *clients;
	EventType       startEvent;
	EventType       endEvent;
} ProviderType;

typedef struct TaskInfoType {
    TaskContextType 	*context;
    QueueHandle_t       *msgQ;
    uint32_t            signal;
    BaseType_t          dogRpt;
            
    void (*init)(void *arg);
    void (*device_init)(void *arg);
    void (*deinit)(void *arg);
    void (*main_loop)(void *arg);
    void (*message_handler)(const MessageDataType *msg);
    void (*reload_func)(void *arg);
    BaseType_t (*is_running)(void);
} TaskInfoType;


/* external */
extern int task_info_regist(TaskContextType *context);
extern int task_info_unregist(TaskContextType *context);
extern int task_info_get_taskname(const int id, const char *name);
extern void task_info_init(void);

extern void PostBroadcastSignal(const TaskIdType taskId, const uint32_t sig);



#define send_broadcast_signal(x)       PostBroadcastSignal(GetTaskIdviaHandle(xTaskGetCurrentTaskHandle()), x)

#endif  /* _HOMESTEAD_TASK_INFO_H_ */