#ifndef __UART_H__
#define __UART_H__

#include <stdint.h>

typedef uint32_t baudrate_t;

void uart_debug_init(baudrate_t baudrate);
void uart_debug_send_char(char c);
void uart_debug_send_string(const char* str);

#endif // __UART_H__
