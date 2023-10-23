/*
 * Uart.cpp
 *
 *  Created on: Jun 9, 2023
 *      Author: tome
 */

#include <stdarg.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <portmacro.h>

#include "Uart.h"


static struct UartType Uart;
static int Handle = 0;

static void uart_tx_callback(const unsigned int num_bytes, void *data);
static void uart_rx_callback(const unsigned int num_bytes, void *data);

static void init(void);
static void deinit(void);
static void init_device(void);
static int is_enabled(void);
static int open(void);
static int close(void);
static int read(char *buf, const unsigned int length);
static int write(const char *buf, const unsigned int length);


static void uart_tx_callback(const unsigned int num_bytes, void *data) {
	//TBD
}

static void uart_rx_callback(const unsigned int num_bytes, void *data) {
	//TBD
}


static void init(void) {
    init_device();
    open();
    Uart.enabled = 1;
}

static void init_device(void) {
}

static void deinit(void) {
    Uart.enabled = 0;
}

static int is_enabled(void) {
    return Uart.enabled;
}

static int open(void) {
    //tbd int result = open(&Handle, Id, &UartConfig);
    int result = 0;

    if (result == 0)
        Uart.enabled = 1;
    return result;
}


static int close(void) {
	Uart.enabled = 0;
    //tbd return (qapi_UART_Close(Handle));
    return 0;
}


static int read(char *buf, const unsigned int length) {
    if ((buf == NULL) || (Uart.enabled == 0) || (length < 1))
        return -1;

    //tbd return qapi_UART_Receive(Handle, buf, UART_BUFFER_SIZE, NULL);
    return 0;
}


static int write(const char *buf, const unsigned int length) {
    int result;
    if ((length == 0) || (buf == NULL) || (Uart.enabled == 0))
        return -1;

    /* Transmit the data. */
    //tbd result = qapi_UART_Transmit(Handle, (char *)buf, length, NULL);
    result = 0;
    
    //Sleep(1);
    vTaskDelay( 1 / portTICK_PERIOD_MS);

    return result;
}


static void set_print_welcome(void) {
	Uart.printWelcome = 1;
}


void CONSOLE_WRITE(const char *format, ...) {
    char buf[200] = {0};
    int len;
    va_list args;

    if (Uart.printWelcome)
    	return;

    va_start(args, format);
    len = vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);
    if ((len == sizeof(buf)) || (len < 0))
        return;

    Uart.write(buf, strlen(buf));
}


void WRITE(const char *format, ...) {
    char buf[200] = {0};
    int len;
    va_list args;

    va_start(args, format);
    len = vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);
    if ((len == sizeof(buf)) || (len < 0))
        return;

    Uart.write(buf, strlen(buf));
}

struct UartType *get_uart(void) {
	return &Uart;
}


void InitializeUart(void) {
    Uart = (struct UartType) {
    	.enabled			= 0,
		.printWelcome 		= 0,

        .init				= init,
        .init_device		= init_device,
        .deinit				= deinit,
        .is_enabled			= is_enabled,
        .open				= open,
        .close				= close,
        .read				= read,
        .write				= write,
		.set_print_welcome 	= set_print_welcome,
    };

    Uart.init();
}
