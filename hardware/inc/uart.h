#ifndef __UART_H__
#define __UART_H__

#include <stdint.h>

typedef uint32_t baudrate_t;

void uart_debug_init(baudrate_t baudrate);
void uart_debug_send_char(char c);
void uart_debug_send_string(const char* str);

void uart_esp32_init(baudrate_t baudrate);
void uart_esp32_send_char(char c);
void uart_esp32_send_string(const char* str);

char uart_esp32_receive_char(void);
uint8_t uart_esp32_available(void);
void uart_esp32_receive_string(char *buffer, uint16_t max_len);
void uart_esp32_receive_bytes(uint8_t *buffer, uint16_t len);

#endif // __UART_H__
