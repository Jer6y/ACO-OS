#include <os.h>
#include <uart.h>


void uart_init()
{
    //关闭中断
    write_reg(IER,0,uint8);
    
    //设置波特率
    write_reg(LCR,1<<7,uint8);
    write_reg(BUAD_LOW,0x3,uint8);
    write_reg(BUAD_HIGH,0x0,uint8);
    
    //设置字模式和校验
    write_reg(LCR,LCR_WORD_LENGTH,uint8);

    //打开FIFO并清空
    write_reg(FCR,FCR_FIFO_ENABLE|FCR_RX_FIFO_CLEAR|FCR_TX_FIFO_CLEAR,uint8);  

    write_reg(IER,IER_RX_INTA|IER_TX_INTA,uint8);
}
void uart_putc_sync(char c)
{
    uint8 state =0;
    while(!((state=read_reg(LSR,uint8))&LSR_TX_READY)) ;
    write_reg(THR,c,uint8);
}

int uart_getchar()
{
    if(read_reg(LSR,uint8)&LSR_RX_READY)
    {
        return read_reg(RHR,char);
    }
    return -1;
}
void uart_putc(char c)
{
    push_on();
    while(!(output_r+OUT_BUF_SIZE<output_w))
    {
        //阻塞目前进程,阻塞原因事是写缓冲区满;
        // sleep(&output_w);
        push_on();
    }
    //临界区
    output_buffer[(output_w++)&(OUT_BUF_SIZE-1)] = c;
    pop_off();
}
void uart_out_intr()
{
    while(output_r!=output_w)
    {
        if(!(read_reg(LSR,uint8)&LSR_TX_READY)) break;
        char c = output_buffer[output_r++];
        write_reg(THR,c,uint8);
    }   
    // wakeup(&output_w);
}

//进入中断处理的时候
//必须关中断
void uart_int_handler()
{
    while(1)
    {
        int c = uart_getchar();
        if(c==-1) break;
        // console_int_handler(c);
    }
    uart_out_intr();
}