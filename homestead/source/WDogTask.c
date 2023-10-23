#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>

#include "WDogTask.h"
#include "Timer.h"
#include "TaskSignal.h"
#include "WDogTask.h"

#include "Log.h"

#include "console.h"

#define MAX_WDOG_QUEUE_SIZE 8

static const char *TaskName = "WDog";
static int TaskPriority = tskIDLE_PRIORITY;
static const int TaskStackSize = (1024 * 2);

/* Context */
static TaskContextType      Context;
static TaskInfoType         TaskInfo;
static WDogTaskType         DogTask;

/* queue */
#define WDOG_MAX_Q_SIZE      10
static QueueHandle_t        wdog_msg_q;

/* service */
static WDogType 			*WDog;

/* Declare prototype of static functions */
static void init(void *arg);
static void device_init(void *arg);
static void deinit(void *arg);
static void main_loop(void *arg);
static void message_handler(const MessageDataType *msg);
static void reload_func(void *arg);
static BaseType_t is_running(void);

static void wdog_send_event(const EventType event, const uint32_t data, const uint32_t data2);

/* timer */
#define WDOG_TIMER_DOG_PERIOD                3000
#define WDOG_TIMER_GENERIC_PERIOD            1000
const char *wdog_timer_name_dog = "wdog_tmr_dog";
const char *wdog_timer_name_audit = "wdog_tmr_audit";
static TimerType   wdog_timer_dog, wdog_timer_audit;

static void wdog_timer_callback(TimerHandle_t timer) {
    configASSERT(timer);

    if (timer == wdog_timer_dog.handle) {
        wdog_send_event(EVT_WDOG_DOG_RPT_REQ, 0, 0);
    } else if (timer == wdog_timer_audit.handle) {
        wdog_send_event(EVT_WDOG_AUDIT_REQ, 0, 0);
    }
}


static void wdog_timer_create(TimerType *timer) {
    configASSERT(timer);
    timer->handle = xTimerCreate(timer->name, timer->period, timer->auto_reload, timer->timer_id, timer->callback);
}


static void wdog_timer_delete(TimerType *timer) {
    configASSERT(timer);
    xTimerDelete(timer->handle, 0);

    timer_clear(timer);
}

static void wdog_timer_start(TimerType *timer) {
    configASSERT(timer);

    if( xTimerStart( timer->handle, 0 ) != pdPASS ) {
        /* The timer could not be set into the Active state. */
        console_print("[%s %04d]\n", __func__, __LINE__);
    }
}

static void wdog_timer_stop(TimerType *timer) {
    configASSERT(timer);
    xTimerStop(timer->handle, 0);
}

static void device_init(void *arg) {
}


/* functions */
static void init(void *arg) {
    /* deviceInit */
    device_init(NULL);

    /* queuq */
    wdog_msg_q = xQueueCreate(WDOG_MAX_Q_SIZE, sizeof(struct MessageDataType));
    if( wdog_msg_q == NULL ) {
        /* Queue was not created and must not be used. */
        console_print("Error: can't create wdog_msg_q\n");
    }

    /* timer */
    wdog_timer_dog = (TimerType) {
        .handle             = NULL,
        .name               = wdog_timer_name_dog,
        .period             = WDOG_TIMER_DOG_PERIOD,
        .auto_reload        = pdTRUE,
        .timer_id           = NULL,
        .callback           = wdog_timer_callback,
    };

    wdog_timer_audit = (TimerType) {
        .handle             = NULL,
        .name               = wdog_timer_name_audit,
        .period             = WDOG_TIMER_GENERIC_PERIOD,
        .auto_reload        = pdTRUE,
        .timer_id           = NULL,
        .callback           = wdog_timer_callback,
    };
    wdog_timer_create(&wdog_timer_dog);
    wdog_timer_create(&wdog_timer_name_audit);
}

static void deinit(void *arg) {
}


static BaseType_t is_running(void) {
    return pdTRUE;
}

static void reload_func(void *arg) {
}


static int32_t wdog_q_send(void *data) {
    BaseType_t result = 0;
 
    result = xQueueSend(wdog_msg_q, data, portMAX_DELAY);
    if (result != pdPASS) {
        /* Failed to post the message, even after 10 ticks. */
        console_print("Error(%d): can't send data\n", result);
        goto RET;
    }
    xTaskNotify(Context.task, TASK_MESSAGE_Q_SIG, eSetValueWithoutOverwrite);

RET:
    return result;
}

static void message_handler(const MessageDataType *msg) {
    configASSERT(msg->event > EVT_NONE && msg->event < EVT_MAX);
    
    log_low("DogTask::Event=0x%x\n", msg->event);
    switch (msg->event) {
    case EVT_WDOG_STARTED_NOTI:
        break;

    case EVT_WDOG_DOG_RPT_REQ:
        log_mid("EVT_WDOG_DOG_RPT_REQ");
        break;

    case EVT_WDOG_AUDIT_REQ:
        log_high("EVT_WDOG_AUDIT_REQ");
        break;

    default:
        break;
    }
}

static void main_loop(void *arg) {
    uint32_t signal;
    MessageDataType msg;
    BaseType_t result;

    log_low("\r\n\r\nStarting DogTask...");

    /* start timers */
    wdog_timer_start(&wdog_timer_dog);
    wdog_timer_start(&wdog_timer_name_audit);

    /* infinte loop */
    for (;;) {
        result = xTaskNotifyWait(pdFALSE, ULONG_MAX, &signal, portMAX_DELAY);
        if (result == pdPASS) {
            if (signal & TASK_EMERGENCY_SIG) {
                //TBD
            }

            if (signal & TASK_WDOG_RELOADED_SIG) {
                //TBD
            }

            if (signal & TASK_MESSAGE_Q_SIG) {
                memset(&msg, 0, sizeof(struct MessageDataType));
                if (xQueueReceive(wdog_msg_q, &msg, portMAX_DELAY) == pdPASS) {
                    message_handler(&msg);
                }
            }

            if (uxQueueMessagesWaiting(wdog_msg_q)) {
                xTaskNotify(Context.task, TASK_MESSAGE_Q_SIG, eSetValueWithoutOverwrite);
            }

            taskYIELD();
        }
    }
}


static void wdog_send_event(const EventType event, const uint32_t data, const uint32_t data2) {
	MessageDataType msg;
	configASSERT(event >= EVT_NONE && event <= EVT_MAX);

	/* message */
	msg.event = event;
	msg.data = data;
	msg.data2 = data2;

	/* destination(s) */
	wdog_q_send((void *)&msg);
}

TaskHandle_t wdog_task_get_handle(void) {
    return Context.task;
}


void create_wdog_task(void) {
	BaseType_t result;
    TaskHandle_t taskHandle = NULL;

    /* WDog */
	wdog_initialize();
    WDog = get_log();
    if (WDog == NULL) {
    	//CONSOLE_WRITE("WDog is NULL\r\n");
        console_print("log is NULL\n");
    }


    /* context */
    Context = (struct TaskContextType) {
    	.id				= TASK_ID_LOG,
        .name			= TaskName,
        .priority		= TaskPriority,
        .stackSize		= TaskStackSize
    };

    /* thread information */
    TaskInfo = (TaskInfoType) {
        .context		    = &Context,
        .msgQ               = &wdog_msg_q,
        .dogRpt             = pdFALSE,

		.init			    = init,
        .device_init		= device_init,
        .deinit			    = deinit,
        .main_loop		    = main_loop,
        .message_handler	= message_handler,
        .reload_func		= reload_func,
        .is_running		    = is_running,
    };

    /* thread */
    DogTask = (struct WDogTaskType) {
        .task		= &TaskInfo,
        .dog		= WDog,
    };

    DogTask.dog->set_task(&TaskInfo);

    // initialize thread
    DogTask.task->init(NULL);

    /* */
    result = task_info_regist(&Context);
    if (result != 0) {
    	//CONSOLE_WRITE("Error. Couldn't register thread in");
    }

    /* Create the task, storing the handle. */
    result = xTaskCreate(
                main_loop,              /* Function that implements the task. */
                Context.name,           /* Text name for the task. */
                Context.stackSize,      /* Stack size in words, not bytes. */
                NULL,                   /* Parameter passed into the task. */
                Context.priority,       /* Priority at which the task is created. */
                &Context.task );        /* Used to pass out the created task's handle. */
}

