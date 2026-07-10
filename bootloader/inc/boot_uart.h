#ifndef BOOT_UART_H
#define BOOT_UART_H

#include <stdint.h>

void boot_uart_init(uint32_t baudrate);
void boot_uart_send_char(char c);
void boot_uart_send_string(const char *str);
void boot_uart_send_hex32(uint32_t value);

#endif
