#include <time.h>
#include <stdarg.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "Log.h"
#include "LogSpool.h"
#include "Uart.h"
#include "Utils.h"
#include "TaskSignal.h"

// posix console print
#include "console.h"

static TaskInfoType		    *Task;
static SemaphoreHandle_t    Mutex = NULL;

static unsigned char 		Initialized;
static unsigned char 		Enabled;
static unsigned char 		Locked;
static uint32_t 			Level;
static uint32_t 			Mask;

static struct LogType Log;

/////////////////////////////
extern QueueType *log_spool_get_object(void);
extern void spool_init(void);
extern TaskHandle_t log_task_get_handle(void);

static void init(void);
static void deinit(void);
static void create_mutex(void);
static void delete_mutex(void);
static unsigned char is_initialized(void);
static void set_initialized(const unsigned char init);
static unsigned char is_enabled(void);
static void set_enabled(const unsigned char enable);
static unsigned char is_locked(void);
static void set_locked(const unsigned char locked);
static uint32_t get_level(void);
static void set_level(const uint32_t id);
static void clear_level(const uint32_t id);
static uint32_t get_mask(void);
static void set_mask(const uint32_t id);
static void clear_mask(const uint32_t id);
static void lock(void);
static void unlock(void);
static void out(const char *msg, const int len);
void log_print(const uint32_t Level, const char *filename, const uint32_t lineNo, const char *fmt, ...);
static void reload_func(void);

static void set_task(TaskInfoType *thread);
//////////////////////

static void init(void) {
	create_mutex();
    set_locked(pdFALSE);
    set_enabled(pdTRUE);
    //set_level();
}


static void deinit(void) {
    set_locked(pdTRUE);
    delete_mutex();
}

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

static unsigned char is_initialized(void)  {
    return Initialized;
}

static void set_initialized(const unsigned char init) {
    Initialized = init;
}

static unsigned char is_enabled(void) {
    return Enabled;
}

static void set_enabled(const unsigned char enable) {
    Enabled = enable;
}

static unsigned char is_locked(void) {
    return Locked;
}

static void set_locked(const unsigned char locked) {
    Locked = locked;
}


static uint32_t get_level(void) {
    return Level;
}


static void set_level(const uint32_t id) {
    Level |= (1 << id);
}

static void clear_level(const uint32_t id) {
    Level &= ~(1 << id);
}

static uint32_t get_mask(void) {
    return Mask;
}

static void set_mask(const uint32_t id) {
    Mask |= (1 << id);
}

static void clear_mask(const uint32_t id) {
    Mask &= ~(1 << id);
}

static void lock(void) {
    xSemaphoreTake( Mutex, portMAX_DELAY );
}

static void unlock(void) {
    xSemaphoreGive( Mutex );
}

static void out(const char *msg, const int len) {
	//Log.uart->write(msg, len);
    console_print(msg);
}


void log_print(const uint32_t Level, const char *filename, const uint32_t lineNo, const char *fmt, ...) {
	size_t len = 0, temp_len = 0;
    char buf[256] = {0, };
    char *ptr = &buf[0];
    char fname[configMAX_TASK_NAME_LEN] = { 0 };
    UpTimeStructType upTime = {0};
    TaskHandle_t task;

    lock();

    /* conditional filtering */
    if (is_initialized() == pdFALSE)
        goto RET;

    if (is_locked())
    	goto RET;

    if (Level < DEBUG_LEVEL_EMERG || Level >= DEBUG_LEVEL_MAX)
    	goto RET;

#if 0  //TBD
    /* level */
    if (Level > getLevel())
    	goto RET;

    /* task filtering */
    if (((getMask() >> taskId) & 0x01) == 0)
    	goto RET;
#endif

    /* filename and line number */
    va_list ap;
    va_start(ap, fmt);

    /* time stamp */
	if (Level <= DEBUG_LEVEL_MID) {
#ifdef CONFIG_FEATURE_RTC_SUPPORT	// we don't have a RTC. So we can't set/get GPS clock.
        struct TimeInfoType timeInfo = GetTime();

        snprintf(ptr, sizeof(buf), "[%02ld-%02ld-%02ld %02ld:%02ld:%02ld] ", timeInfo.year, timeInfo.month, timeInfo.day, timeInfo.hour, timeInfo.min, timeInfo.sec);
        len = strlen((char *)&buf);
        ptr += len;
#else	// we can get uptime.
        GetUpTime(&upTime);
        snprintf(ptr, sizeof(buf), "[%03d %02d:%02d:%02d.%03d] ", upTime.days, upTime.hours, upTime.minutes, upTime.seconds, upTime.milliseconds);
        len = strlen((char *)&buf);
        ptr += len;
#endif
	}

    /* filename & line number */
    if (Level <= DEBUG_LEVEL_HIGH) {
		memset(fname, 0x0, sizeof(fname));
		GetFilename((char *)(filename), fname);
		snprintf(ptr, sizeof(buf), "[%s %04d] ", fname, lineNo);
		len = strlen((char *)&buf);
		ptr += len;
    }

    /* log body */
	if (len >= sizeof(buf))
		len = sizeof(buf);
	temp_len = (sizeof(buf) - len - 1) < 0 ? 0 : (sizeof(buf) - len - 1);
	vsnprintf((char *)&buf + len, temp_len , fmt, ap);
    va_end(ap);

    /* add CRLF */
    len = strlen((char *)&buf);
    if (len >= (LOG_SIZE - 2)) {
        len = (LOG_SIZE - 2);
        buf[len] = '\0';
    }

    if (strncmp((char *)&buf[len - 2], "\r\n", 2) == 0) {
        // nothing to do
    } else if (buf[len - 1] == '\r') {
        buf[len] = '\n';
        buf[len + 1] = '\0';
        len += 1;
    } else if (buf[len - 2] != '\r' && buf[len - 1] == '\n') {
        buf[len - 1] = '\r';
        buf[len] = '\n';
        buf[len + 1] = '\0';
        len += 2;
    } else {
        buf[len] = '\r';
        buf[len + 1] = '\n';
        buf[len + 2] = '\0';
        len += 2;
    }

    /* push log to buffer */
    Log.spool->push(buf, len);

    /* signaling */
    task = log_task_get_handle();
    //xTaskNotify(Log.task->context->task, TASK_LOG_SPOOL_SIG, eSetValueWithoutOverwrite);
    xTaskNotify(task, TASK_LOG_SPOOL_SIG, eSetValueWithoutOverwrite);

RET:
	unlock();
}

static void reload_func(void) {
	log_low("Log is ready!");
}

static void set_task(TaskInfoType *task) {
    Task = task;
}

LogType *get_log(void) {
    return &Log;
}

void log_initialize(void) {

    spool_init();

    Log = (struct LogType) {
        .task			    = Task,
        .spool			    = log_spool_get_object(),
		.uart			    = get_uart(),

        .set_task		    = set_task,
        .init			    = init,
        .deinit			    = deinit,
        .create_mutex	    = create_mutex,
        .delete_mutex	    = delete_mutex,
        .is_initialized	    = is_initialized,
        .set_initialized	= set_initialized,
        .is_enabled		    = is_enabled,
        .set_enabled		= set_enabled,
        .is_locked		    = is_locked,
        .set_locked		    = set_locked,
        .get_level		    = get_level,
        .set_level		    = set_level,
        .clear_level		= clear_level,
        .get_mask		    = get_mask,
        .set_mask		    = set_mask,
        .clear_mask		    = clear_mask,
        .lock			    = lock,
        .unlock			    = unlock,
        .out			    = out,
        .print			    = log_print,
        .reload_func		= reload_func
    };
    Log.init();
    Log.reload_func();
}
