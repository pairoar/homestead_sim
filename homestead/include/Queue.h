#ifndef _HOMESTEAD_QUEUE_H_
#define _HOMESTEAD_QUEUE_H_

#include <stdint.h>

typedef struct QueueContextType {
    void                *handle;
    void                *attribute;
	uint32_t            slots;
    uint32_t            size;
} QueueContextType;


typedef struct QueueType {
    void *context;

    void (*init)(uint32_t slots, uint32_t size);
    void (*deinit)(void);
    void (*lock)(void);
    void (*unlock)(void);
    void (*push)(const char *data, const uint16_t len);
    void (*pop)(char *data, uint16_t* len);
    BaseType_t (*is_empty)(void);
    BaseType_t (*is_full)(const uint16_t len);
    void (*clear)(void);
} QueueType;

#endif  /* _HOMESTEAD_QUEUE_H_ */