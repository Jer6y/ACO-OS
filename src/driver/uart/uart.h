/*
 * uart 驱动头文件
 * 存放  uart驱动的各个函数等
 * uart 为 virt模拟的16550a
 */
#ifndef UART_H
#define UART_H

typedef int (*uart_rcv_handler)(int c);

void uart_hook_rcv_handler(uart_rcv_handler handler);

void uartinit(void);

void uartstart();

int uart_intr(void);

void uartputc_sync(int c);

void uartputstr_sync(char* s, int size);

int printk(const char* str,...);


#endif