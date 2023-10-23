
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "WDog.h"
#include "Event.h"
#include "TaskSignal.h"

static TaskInfoType		    *Task;

static WDogContextType Context;
static WDogType WDog;
static QueueHandle_t *msgHandle;

static ProviderType WDogProvider = {
    .taskId         = TASK_ID_DOG,
    .type           = PROVIDER_DOG,
	.clients        = &Context.clients,
    .startEvent     = EVT_WDOG_SERVICE_REGISTERED_NOTI,
    .endEvent       = EVT_WDOG_SERVICE_UNREGISTERED_NOTI,
};


/* Definitions for mutexWDog */
static SemaphoreHandle_t Mutex;


/* function prototype of mutext */
static void create_mutex(void);
static void delete_mutex(void);
static void lock(void);
static void unlock(void);

/* function prototype of context */
static uint32_t get_clients(void);
static uint32_t get_monitoring(void);
static uint32_t get_reported(void);
static int is_ready(void);

/* function prototype of wdog */
static void init(void);
static void deinit(void);
static int regist(const TaskIdType taskId);
static int unregist(const TaskIdType taskId);
static int resume(const TaskIdType taskId);
static int suspend(const TaskIdType taskId);
static void set_reported(const TaskIdType taskId);
static void clear(void);
static void send_reloading(void);
static void audit(void);
static int is_ready(void);
static void set_task(TaskHandle_t *handle);

WDogType* dog_get_service(void);
void dog_send_report(QueueHandle_t *from, const TaskIdType taskId);
void dog_initialize(void);

//--------------------------------------------------------------------
static void create_mutex(void) {
    vSemaphoreCreateBinary(Mutex);

    if( Mutex == NULL ) {
       /* The semaphore was created successfully and can be used. */
       console_print("Error: can't create mutex\n");
    }
}

static void delete_mutex(void) {
  vSemaphoreDelete(Mutex);
}

static void lock(void) {
    xSemaphoreTake( Mutex, portMAX_DELAY );
}

static void unlock(void) {
    xSemaphoreGive( Mutex );
}


static void init(void) {
	Context = (WDogContextType ) {
		.clients			= 0,
		.monitoring			= 0,
		.reported			= 0,
	 	.isReady			= 0,
		.count				= 0,
	};
	create_mutex();
}

static void deinit(void) {
	delete_mutex();

	Context.isReady = 0;
	Context.monitoring = 0;
	Context.reported = 0;
	Context.count = 0;
}


static int regist(const TaskIdType taskId) {
	configASSERT(taskId >= 0 && taskId < TASK_ID_MAX);

	lock();
	Context.clients |= (1 << taskId);
	Context.monitoring |= (1 << taskId);
	Context.count++;
	unlock();

	return 0;
}


static int unregist(const TaskIdType taskId) {
	configASSERT(taskId >= 0 && taskId < TASK_ID_MAX);

	lock();
	Context.clients &= ~(1 << taskId);
	Context.monitoring &= ~(1 << taskId);
	Context.count--;
	unlock();

	return 0;
}


static int resume(const TaskIdType taskId) {
	configASSERT(taskId >= 0 && taskId < TASK_ID_MAX);

	lock();
	Context.monitoring |= (1 << taskId);
	unlock();

	return 0;
}


static int suspend(const TaskIdType taskId) {
	configASSERT(taskId >= 0 && taskId < TASK_ID_MAX);

	lock();
	Context.monitoring &= ~(1 << taskId);
	unlock();

	return 0;
}


static void set_reported(const TaskIdType taskId) {
	configASSERT(taskId >= 0 && taskId < TASK_ID_MAX);

	lock();
	Context.reported |= (1 << taskId);
	unlock();
}


static void clear(void) {
	lock();
	Context.reported = 0;
	unlock();
}


static void send_reloading(void) {
	send_broadcast_signal(TASK_WDOG_RELOADED_SIG);
}

/*
 *  TBD
 *
 */
static void audit(void) {
	if (Context.reported == Context.monitoring) {
		// kick
		//  HAL_IWDG_Refresh(&hiwdg);

		/* notify all of task that watchdog should be reloaded */
		send_reloading();
	} else {
		// print which task report and or not


		// finally call H/W watchdog or custom reset
		// If you don't like that, at least reset it  programatically.
		//    NVIC_SystemReset();
	}
}


/* context supporting functions */
static uint32_t get_clients(void) {
	return Context.clients;
}


static uint32_t get_monitoring(void) {
	return Context.monitoring;
}

static uint32_t get_reported(void) {
	return Context.reported;
}

static int is_ready(void) {
	return Context.isReady;
}


static void set_task(TaskHandle_t *handle) {
	configASSERT(handle);

	WDog.task = handle;
}

static void set_message_q(QueueHandle_t *handle) {
	if (handle == NULL)
		return;
	msgHandle = handle;
}


void wdog_send_report(QueueHandle_t *from, const TaskIdType taskId) {
	//SEND_SIMPLE_MESSAGE(*msgHandle, *from, EVT_WDOG_RPT, taskId, 0);
}


WDogType* wdog_get_service(void) {
	return &WDog;
}

void wdog_initialize(void) {
    WDog = (WDogType ) {

        .init				= init,
        .deinit				= deinit,
        .regist				= regist,
        .unregist			= unregist,
        .is_ready			= is_ready,
        .get_clients			= get_clients,
        .set_reported		= set_reported,
        .audit				= audit,
		.send_reloading 		= send_reloading,
       	.set_task			= set_task,
        .set_message_q		= set_message_q,
    };

	WDog.init();
	Context.isReady = 1;
}
