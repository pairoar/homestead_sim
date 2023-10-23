
#include <string.h>

#include "Timer.h"
#include "Utils.h"

void GetFilename(const char *input, char *output) {
    int tail = 0, len = 0;

    configASSERT(input);
    configASSERT(output);

    while (input[tail] != 0 && input[tail] != '\0')
        tail++;

    while (input[tail - len] != '/' && (tail - len) > 0)
        len++;

    strncpy(output, &input[tail - len + 1], (len  < configMAX_TASK_NAME_LEN) ? len : configMAX_TASK_NAME_LEN);
}


#ifdef CONFIG_FEATURE_RTC_SUPPORT
struct TimeInfoType GetTime(void) {
    struct TimeInfoType timeInfo;
    time_t currTime;
    struct tm* pTimeInfo;

    currTime = time(NULL);
    pTimeInfo = localtime(&currTime);

    timeInfo.year = pTimeInfo->tm_year + 1900;
    timeInfo.month = pTimeInfo->tm_mon + 1;
    timeInfo.day = pTimeInfo->tm_mday;
    timeInfo.hour = pTimeInfo->tm_hour;
    timeInfo.min = pTimeInfo->tm_min;
    timeInfo.sec = pTimeInfo->tm_sec;

    return timeInfo;
}
#endif


uint32_t GetUpTime(UpTimeStructType *time) {
    uint32_t ticks, ms;

    ticks = (uint32_t)xTaskGetTickCount();
    ms = ticks;

    if (time) {
        time->ticks = ticks;
        time->milliseconds = ms % 1000;
        time->seconds = ms / 1000;

        time->days = time->seconds / 86400;
    	time->hours = (time->seconds / 3600) % 24;
    	time->minutes = (time->seconds / 60) % 60;
    	time->seconds %= 60;
    }

    return 0;
}
