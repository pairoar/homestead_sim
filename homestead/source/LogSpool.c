/*
 * LogSpool.c
 *
 *  Created on: Jun 13, 2023
 *      Author: tome
 */
#include <FreeRTOS.h>

#include "LogSpool.h"
#include "Uart.h"
//#include "Utils.h"

/* Declare variables */
static SemaphoreHandle_t Mutex;
static uint8_t 			Buffer[LOG_BUFFER_SIZE];

static LogSpoolContextType Context;
static QueueType LogSpool;

/* Declare prototype of static functions */
static void init(uint32_t slots, uint32_t size);
static void deinit(void);
static void lock(void);
static void unlock(void);
static void push(const char *data, const uint16_t len);
static void pop(char *data, uint16_t* len);
static BaseType_t is_empty(void);
static BaseType_t is_full(const uint16_t len);
static void clear(void);

static void create_mutex(void) {
	Mutex = xSemaphoreCreateMutex();

  if( Mutex != NULL ) {
       /* The semaphore was created successfully and can be used. */
  }
}

static void delete_mutex(void) {
  vSemaphoreDelete(Mutex);
  Mutex = NULL;
}


/* functions */
static void init(uint32_t slots, uint32_t size) {
    create_mutex();
    clear();
}

static void deinit(void) {
    delete_mutex();
}


static void lock(void) {
    if( xSemaphoreTake( Mutex, ( TickType_t ) 10 ) == pdTRUE ) {
          
    } else {
        /* We could not obtain the semaphore and can therefore not access
            the shared resource safely. */
    }
}

static void unlock(void) {
    xSemaphoreGive( Mutex );
}

static void push(const char *data, const uint16_t len) {
    uint16_t size = len;

    lock();
    if (is_full(len))
    	goto RET;

    if ((Context.in + len) >= LOG_BUFFER_SIZE) {
        size = LOG_BUFFER_SIZE - Context.in;
        memcpy((uint8_t *)&Buffer[Context.in], data, size);
        memcpy((uint8_t *)&Buffer[0], &data[size], len - size);
        Context.in = len - size;
    } else {
        memcpy((uint8_t *)&Buffer[Context.in], data, len);
        Context.in += len;
    }

    Buffer[Context.in++] = DELIMITER;
RET:
    unlock();
}

static void pop(char *data, uint16_t* len) {
	lock();
	*data = NULL;
	*len = 0;
	if (is_empty())
		goto RET;

    uint8_t *ptr = (uint8_t *)&Buffer[Context.out];

    while (*ptr) {
        *data++ = *ptr++;
        (*len)++;
        Context.out++;

        if (Context.out >= LOG_BUFFER_SIZE) {
            ptr = (uint8_t *)&Buffer[0];
            Context.out = 0;
        }
    }
    *data = DELIMITER;
    Context.out++;

RET:
	unlock();
}

static BaseType_t is_empty(void) {
    return  (Context.in == Context.out);
}

static BaseType_t is_full(const uint16_t len) {
	if (Context.in < Context.out) {
		return (Context.in + len) > Context.out;
	} else {
		return (Context.in + len) > (Context.out + LOG_BUFFER_SIZE);
	}
}


static void clear(void) {
	lock();
    Context.in = 0;
    Context.out = 0;
    memset((uint8_t *)&Buffer, 0, LOG_BUFFER_SIZE);
    unlock();
}

QueueType *log_spool_get_object(void) {
    return &LogSpool;
}


void spool_init(void) {
    /* context */
    Context = (LogSpoolContextType) {
        .in			= 0,
        .out		= 0,
        .mutex		= &Mutex,
        .buffer		= (uint8_t *)&Buffer[0],
    };
    memset((uint8_t *)&Buffer, 0, LOG_BUFFER_SIZE);

    /* spool */
    LogSpool = (struct QueueType) {
        .context	= &Context,

        .init		= init,
        .deinit		= deinit,
        .lock		= lock,
        .unlock		= unlock,
        .push		= push,
        .pop		= pop,
        .is_empty	= is_empty,
        .is_full		= is_full,
        .clear		= clear,
    };
    LogSpool.init(0, 0);
}

