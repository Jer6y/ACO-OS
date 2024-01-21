/*
 * uart 驱动头文件
 * 存放  uart驱动的各个函数等
 * uart 为 virt模拟的16550a
 */
#ifndef UART_H
#define UART_H


void uartstart();

int uart_intr(void);

void uartputc_sync(int c);

#endif