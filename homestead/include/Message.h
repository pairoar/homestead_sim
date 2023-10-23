#ifndef _HOMESTEAD_MESSAGE_H_
#define _HOMESTEAD_MESSAGE_H_

#include <stdint.h>

#include "Event.h"

typedef struct MessageDataType {
    EventType       event;
    uint32_t        data;
    uint32_t        data2;
} MessageDataType;

#endif  /* _HOMESTEAD_MESSAGE_H_ */