#ifndef _HOMESTEAD_UTILS_H_
#define _HOMESTEAD_UTILS_H_

#include <stdint.h>


#include <FreeRTOS.h>
#include <task.h>

struct TimeInfoType {
    int year;
    int month;
    int day;
    int hour;
    int min;
    int sec;
};

typedef struct UpTimeStructType {
    uint32_t    ticks;          /* number of systicks */
    uint32_t    milliseconds;   /* number of milliseconds */
    uint32_t    seconds;        /* number of seconds */
    uint32_t    minutes;        /* number of minutes */
    uint32_t    hours;        	/* number of hours */
    uint32_t    days;        	/* number of days */
} UpTimeStructType;

#ifdef CONFIG_FEATURE_RTC_SUPPORT
extern struct TimeInfoType GetTime(void);
#endif

extern void GetFilename(const char *input, char *output);
extern uint32_t GetUpTime(UpTimeStructType *time);


#endif  /* _HOMESTEAD_UTILS_H_ */