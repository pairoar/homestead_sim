
#ifndef _HOMESTEAD_TIMER_H_
#define _HOMESTEAD_TIMER_H_

#include <FreeRTOS.h>
#include <timers.h>

typedef struct TimerType {
    TimerHandle_t           handle;
    const char              *name;
    TickType_t              period;
    UBaseType_t             auto_reload;
    void*                   timer_id;
    TimerCallbackFunction_t callback;
} TimerType;


extern void timer_clear(TimerType *timer);

#endif  /* _HOMESTEAD_TIMER_H_ */