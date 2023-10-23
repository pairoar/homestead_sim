#include "Timer.h"

void timer_clear(TimerType *timer) {
    timer->handle             = NULL;
    timer->timer_id           = NULL;
    timer->callback           = NULL;
}