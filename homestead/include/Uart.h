#ifndef _HOMESTEAD_UART_H_
#define _HOMESTEAD_UART_H_

#include <stdint.h>

#define UART_BUFFER_SIZE 2048

typedef struct UartType {
    unsigned char 				enabled;
    unsigned char				printWelcome;

    void (*init)(void);
    void (*init_device)(void);
    void (*deinit)(void);
    int (*is_enabled)(void);
    int (*open)(void);
    int (*close)(void);
    int (*read)(char *buf, const uint32_t length);
    int (*write)(const char *buf, const uint32_t length);
    void (*set_print_welcome)(void);

    void (*rx_callback)(const unsigned int num_bytes, void *data);
    void (*tx_callback)(const unsigned int num_bytes, void *data);
} UartType;

extern struct UartType *get_uart(void);


#endif  /* _HOMESTEAD_UART_H_ */