#ifndef _HOMESTEAD_LOG_H_
#define _HOMESTEAD_LOG_H_

#include <string.h>

#include "Queue.h"
#include "Event.h"
#include "LogSpool.h"
#include "Uart.h"
#include "TaskInfo.h"


enum {
    DEBUG_LEVEL_EMERG = 0,
    DEBUG_LEVEL_ALERT,
    DEBUG_LEVEL_CRIT,
    DEBUG_LEVEL_ERR,
    DEBUG_LEVEL_WARNING,
    DEBUG_LEVEL_NOTICE,
    DEBUG_LEVEL_INFO,
    DEBUG_LEVEL_DEBUG,
	DEBUG_LEVEL_STATISTICS,
    DEBUG_LEVEL_MAX,
    DEBUG_LEVEL_UNKNOWN = DEBUG_LEVEL_MAX,
};

#define DEBUG_LEVEL_LOW DEBUG_LEVEL_DEBUG
#define DEBUG_LEVEL_MID DEBUG_LEVEL_INFO
#define DEBUG_LEVEL_HIGH DEBUG_LEVEL_NOTICE

#if 0
#define DBG_MASK_ALL_MASK (
		TASK_MASK_DOG | \
		TASK_MASK_LOG | \
		TASK_MASK_PWR | \
)

#define DBG_MASK_DEFAULT (
        TASK_MASK_MAIN | \
		TASK_MASK_DOG | \
		TASK_MASK_LOG | \
		TASK_MASK_PWR )
#else

#define DBG_MASK_ALL_MASK 0xFFFF
#define DBG_MASK_DEFAULT 0xFFFF

#endif

#define LOG_SIZE 128

typedef struct LogReqType {
    EventType 	    event;
    uint16_t 	    length;
    char 	        buf[LOG_SIZE];
} LogReqType;


typedef struct LogType {
    TaskInfoType		*task;
    SemaphoreHandle_t   *mutex;

    UartType			*uart;
    QueueType			*spool;

    unsigned char 		*enabled;
    unsigned char 		*locked;
    uint32_t 			*level;
    uint32_t 			*mask;

    void (*set_task)(TaskInfoType *task);

    void (*init)(void);
    void (*deinit)(void);
    void (*create_mutex)(void);
    void (*delete_mutex)(void);
    unsigned char (*is_initialized)(void);
    void (*set_initialized)(const unsigned char init);
    unsigned char (*is_enabled)(void);
    void (*set_enabled)(const unsigned char enable);

    unsigned char (*is_locked)(void);
    void (*set_locked)(const unsigned char locked);
    uint32_t (*get_level)(void);
    void (*set_level)(const uint32_t id);
    void (*clear_level)(const uint32_t id);
    uint32_t (*get_mask)(void);
    void (*set_mask)(const uint32_t id);
    void (*clear_mask)(const uint32_t id);
    void (*lock)(void);
    void (*unlock)(void);
    void (*out)(const char *msg, const int len);
    void (*print)(const uint32_t level, const char *filename, const uint32_t line_no, const char *fmt, ...);
    void (*reload_func)(void);
} LogType;


#define log_debug(msg, ...)     log_print(DEBUG_LEVEL_DEBUG, __FILE__, __LINE__, msg, ##__VA_ARGS__)
#define log_info(msg, ...) 	    log_print(DEBUG_LEVEL_INFO,  __FILE__, __LINE__, msg, ##__VA_ARGS__)
#define log_warn(msg, ...) 	    log_print(DEBUG_LEVEL_WARNING, __FILE__, __LINE__, msg, ##__VA_ARGS__)
#define log_error(msg, ...) 	log_print(DEBUG_LEVEL_ERR, __FILE__, __LINE__, msg, ##__VA_ARGS__)
#define log_dump(msg, ...) 	    log_print(DEBUG_LEVEL_ERR, __FILE__, __LINE__, msg, ##__VA_ARGS__)

#define log_high(msg, ...) 	    log_print(DEBUG_LEVEL_HIGH, __FILE__, __LINE__, msg, ##__VA_ARGS__)
#define log_mid(msg, ...) 	    log_print(DEBUG_LEVEL_MID, __FILE__, __LINE__, msg, ##__VA_ARGS__)
#define log_low(msg, ...) 	    log_print(DEBUG_LEVEL_LOW, __FILE__, __LINE__, msg, ##__VA_ARGS__)

extern void log_print(const uint32_t Level, const char *filename, const uint32_t line_no, const char *fmt, ...);
extern LogType *get_log(void);
extern void log_initialize(void);


#endif  /* _HOMESTEAD_LOG_H_ */