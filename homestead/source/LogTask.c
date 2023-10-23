#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>

#include "Timer.h"
#include "TaskSignal.h"
#include "TaskInfo.h"
#include "LogTask.h"

#include "console.h"

#define MAX_LOG_QUEUE_SIZE 8

static const char *TaskName = "WDog";
static int TaskPriority = tskIDLE_PRIORITY;
static const int TaskStackSize = (1024 * 4);

/* Context */
static TaskContextType      Context;
static TaskInfoType         TaskInfo;
static LogTaskType          LogTask;

/* queue */
#define LOG_MAX_Q_SIZE      10
static QueueHandle_t        log_msg_q;

/* service */
static LogType 			    *WDog;

/* Declare prototype of static functions */
static void init(void *arg);
static void device_init(void *arg);
static void deinit(void *arg);
static void main_loop(void *arg);
static void message_handler(const MessageDataType *msg);
static void reload_func(void *arg);
static BaseType_t is_running(void);

static void log_send_event(const EventType event, const uint32_t data, const uint32_t data2);

/* timer */
#define LOG_TIMER_DOG_PERIOD                3000
#define LOG_TIMER_GENERIC_PERIOD            1000
const char *log_timer_name_dog = "log_tmr_dog";
const char *log_timer_name_generic = "log_tmr_generic";
static TimerType   log_timer_dog, log_timer_generic;

static void log_timer_callback(TimerHandle_t timer) {
    configASSERT(timer);

    if (timer == log_timer_dog.handle) {
        log_send_event(EVT_LOG_DOG_RPT_REQ, 0, 0);
    } else if (timer == log_timer_generic.handle) {
        log_send_event(EVT_LOG_CHECK_REQ, 0, 0);
    }
}


static void log_timer_create(TimerType *timer) {
    configASSERT(timer);
    timer->handle = xTimerCreate(timer->name, timer->period, timer->auto_reload, timer->timer_id, timer->callback);
}


static void log_timer_delete(TimerType *timer) {
    configASSERT(timer);
    xTimerDelete(timer->handle, 0);

    timer_clear(timer);
}

static void log_timer_start(TimerType *timer) {
    configASSERT(timer);

    if( xTimerStart( timer->handle, 0 ) != pdPASS ) {
        /* The timer could not be set into the Active state. */
        console_print("[%s %04d]\n", __func__, __LINE__);
    }
}

static void log_timer_stop(TimerType *timer) {
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
    log_msg_q = xQueueCreate(LOG_MAX_Q_SIZE, sizeof(struct MessageDataType));
    if( log_msg_q == NULL ) {
        /* Queue was not created and must not be used. */
        console_print("Error: can't create log_msg_q\n");
    }

    /* timer */
    log_timer_dog = (TimerType) {
        .handle             = NULL,
        .name               = log_timer_name_dog,
        .period             = LOG_TIMER_DOG_PERIOD,
        .auto_reload        = pdTRUE,
        .timer_id           = NULL,
        .callback           = log_timer_callback,
    };

    log_timer_generic = (TimerType) {
        .handle             = NULL,
        .name               = log_timer_name_generic,
        .period             = LOG_TIMER_GENERIC_PERIOD,
        .auto_reload        = pdTRUE,
        .timer_id           = NULL,
        .callback           = log_timer_callback,
    };
    log_timer_create(&log_timer_dog);
    log_timer_create(&log_timer_generic);
}

static void deinit(void *arg) {
}


static BaseType_t is_running(void) {
    return pdTRUE;
}

static void reload_func(void *arg) {
}

static void spool(LogReqType *log) {
    configASSERT(log->event > EVT_LOG_NONE && log->event < EVT_LOG_MAX);
    WDog->out(log->buf, log->length);
}

static int32_t log_q_send(void *data) {
    BaseType_t result = 0;
 
    result = xQueueSend(log_msg_q, data, portMAX_DELAY);
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
    
    log_low("LogTask::Event=0x%x\n", msg->event);
    switch (msg->event) {
    case EVT_LOG_STARTED_NOTI:
        break;

    case EVT_LOG_PRINT_REQ:
        break;

    case EVT_LOG_DOG_RPT_REQ:
        log_mid("EVT_LOG_DOG_RPT_REQ");
        break;

    case EVT_LOG_CHECK_REQ:
        log_high("EVT_LOG_CHECK_REQ");
        break;

    default:
        break;
    }
}

static void main_loop(void *arg) {
    uint32_t signal;
    LogReqType log;
    MessageDataType msg;
    BaseType_t result;

    log_debug("\r\n\r\nStarting LogTask...");

    /* start timers */
    log_timer_start(&log_timer_dog);
    log_timer_start(&log_timer_generic);

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
                if (xQueueReceive(log_msg_q, &msg, portMAX_DELAY) == pdPASS) {
                    message_handler(&msg);
                }
            }

            if (signal & TASK_LOG_SPOOL_SIG) {
                memset(&log, 0, sizeof(struct LogReqType));
                if (WDog->spool->is_empty() == 0) {
                    WDog->spool->pop(log.buf, &log.length);
                    log.event = EVT_LOG_PRINT_REQ;
                    spool(&log);
                }
            }
            if (WDog->spool->is_empty() == 0) {
                xTaskNotify(Context.task, TASK_LOG_SPOOL_SIG, eSetValueWithoutOverwrite);
            }

            taskYIELD();
        }
    }
}


static void log_send_event(const EventType event, const uint32_t data, const uint32_t data2) {
	MessageDataType msg;
	configASSERT(event >= EVT_NONE && event <= EVT_MAX);

	/* message */
	msg.event = event;
	msg.data = data;
	msg.data2 = data2;

	/* destination(s) */
	log_q_send((void *)&msg);
}

TaskHandle_t log_task_get_handle(void) {
    return Context.task;
}


void create_log_task(void) {
	BaseType_t result;
    TaskHandle_t taskHandle = NULL;

    /* WDog */
	log_initialize();
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
        .msgQ               = &log_msg_q,
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
    LogTask = (struct LogTaskType) {
        .task		= &TaskInfo,
        .log		= WDog,
    };

    LogTask.log->set_task(&TaskInfo);

    // initialize thread
    LogTask.task->init(NULL);

    /* */
    result = task_info_regist(&Context);
    if (result != 0) {
    	//CONSOLE_WRITE("Error. Couldn't register thread in");
    }

    LogTask.log->set_initialized(pdTRUE);

    /* Create the task, storing the handle. */
    result = xTaskCreate(
                main_loop,              /* Function that implements the task. */
                Context.name,           /* Text name for the task. */
                Context.stackSize,      /* Stack size in words, not bytes. */
                NULL,                   /* Parameter passed into the task. */
                Context.priority,       /* Priority at which the task is created. */
                &Context.task );        /* Used to pass out the created task's handle. */
}

