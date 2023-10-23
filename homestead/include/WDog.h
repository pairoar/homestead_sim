#ifndef _HOMESTEAD_WDOG_H_
#define _HOMESTEAD_WDOG_H_

#include <stdint.h>

#include <FreeRTOS.h>
#include <task.h>

#include "TaskInfo.h"

typedef struct WDogContextType {
	uint32_t 	    clients;
	uint32_t 	    monitoring;
	uint32_t 	    reported;

	BaseType_t 	    isReady;
	int 		    count;
} WDogContextType;


typedef struct WDogType {
	TaskInfoType    *task;

	void (*init)(void);
	void (*deinit)(void);
	int (*regist)(const TaskIdType id);
	int (*unregist)(const TaskIdType id);
	int (*is_ready)(void);
	uint32_t (*get_clients)(void);
	void (*set_reported)(const TaskIdType taskId);
	void (*audit)(void);
	void (*send_reloading)(void);

	void (*set_task)(TaskHandle_t *handle);
	void (*set_message_q)(QueueHandle_t *handle);
} WDogType;


extern void dog_send_report(QueueHandle_t *from, const TaskIdType taskId);
extern WDogType* dog_get_service(void);
extern void wdog_initialize(void);


#endif  /* _HOMESTEAD_WDOG_H_ */