#include "Los_type.h"
#include "Los_err.h"
#include "platform.h"
#include "stdarg.h"
#include "string.h"
//上面是依赖 [依赖哪些公共库和哪些模块]
//下面是实现 [实现了哪些公共库或者哪些模块]
#include "uart.h"

uart_rcv_handler rcv_handler;
#define Reg(reg) ((volatile unsigned char *)(UART0 + reg))

#define RHR 0                 // receive holding register (for input bytes)
#define THR 0                 // transmit holding register (for output bytes)
#define IER 1                 // interrupt enable register
#define IER_RX_ENABLE (1<<0)
#define IER_TX_ENABLE (1<<1)
#define FCR 2                 // FIFO control register
#define FCR_FIFO_ENABLE (1<<0)
#define FCR_FIFO_CLEAR (3<<1) // clear the content of the two FIFOs
#define ISR 2                 // interrupt status register
#define LCR 3                 // line control register
#define LCR_EIGHT_BITS (3<<0)
#define LCR_BAUD_LATCH (1<<7) // special mode to set baud rate
#define LSR 5                 // line status register
#define LSR_RX_READY (1<<0)   // input is waiting to be read from RHR
#define LSR_TX_IDLE (1<<5)    // THR can accept another character to send

#define ReadReg(reg) (*(Reg(reg)))
#define WriteReg(reg, v) (*(Reg(reg)) = (v))


void uartinit(void)
{
  // disable interrupts.
  WriteReg(IER, 0x00);

  // special mode to set baud rate.
  WriteReg(LCR, LCR_BAUD_LATCH);

  // LSB for baud rate of 38.4K.
  WriteReg(0, 0x03);

  // MSB for baud rate of 38.4K.
  WriteReg(1, 0x00);

  // leave set-baud mode,
  // and set word length to 8 bits, no parity.
  WriteReg(LCR, LCR_EIGHT_BITS);

  // reset and enable FIFOs.
  WriteReg(FCR, FCR_FIFO_ENABLE | FCR_FIFO_CLEAR);

  // enable transmit and receive interrupts.
  WriteReg(IER, IER_RX_ENABLE);

  rcv_handler = NULL;
}


void uart_hook_rcv_handler(uart_rcv_handler handler)
{
   rcv_handler = handler;
}

void uartputc_sync(int c)
{
  while((ReadReg(LSR) & LSR_TX_IDLE) == 0) ;
  WriteReg(THR, c);
}

int uartgetc(void)
{
  if(ReadReg(LSR) & 0x01)
  {
    // input data is ready.
    return ReadReg(RHR);
  } 
  else 
  {
    return -1;
  }
}

int uart_intr(void)
{
    int ch = uartgetc();
    if(ch != -1) 
    {
      if(rcv_handler != NULL)
      {
        return rcv_handler(ch);
      }
      return -1;
    }
    return -1;
}

void uartputstr_sync(char* s, int size)
{
  for(int i=0;i<size;i++)
  {
      uartputc_sync(s[i]);
  }
}

int printk(const char* str,...)
{
    va_list vlist;
    uint64 s0_addr;
    asm volatile("sd s0, %0":"=m"(s0_addr)::"memory");
    s0_addr += 8;
    va_start(vlist,s0_addr);
    char tmp_buf[256];
    int i = vsprintf(tmp_buf,str,vlist);
    uartputstr_sync(tmp_buf,i);
    return i;
}