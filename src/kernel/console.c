#include <os.h>


#define BACK_SPACE   8
#define PROCESS_DUMP 16
#define EXIT         3

void uartInt_output(char c)
{
    switch(c)
    {
        case BACK_SPACE:
        uart_putc_sync('\b');
        uart_putc_sync(' ');
        uart_putc_sync('\b');
        return;
        case '\r':
        uart_putc_sync('\n');
        return;
        case EXIT:
        return;
        case PROCESS_DUMP:
        return;
    }
    uart_putc_sync(c);
}

void console_int_handler(char c)
{
    uartInt_output(c);
    if(input_w < (input_r+INPUT_BUF_SIZE))
    {
        switch(c)
        {
            case BACK_SPACE:
            if(input_w!=input_r)
            {
                if((--input_w)==input_e) input_e--;
                input_buf[input_w] =0;
            }
            break;
            case '\r':
            case EXIT:
            {
                if(input_r!=input_w)
                {
                    input_e=input_w-1;
                    wakeup(&input_r);
                }
            }
            break;
            case PROCESS_DUMP:
            break;
            default:
            {
                input_buf[input_w++] = c;
                wakeup(&input_r);
            }
        }
    }
}

int console_write(char *str,int lenth)
{
    for(int i=0;i<lenth;i++)
    {
        uart_putc(str[i]);
    }
    return lenth;
}

int console_read(char *des,int lenth)
{
    push_on();
    while(!((input_r+lenth<=input_w)||(input_r+lenth>input_w && input_e!=input_r)))
    {
        sleep(&input_r);
        push_on();
    }
    int i;
    for(i=0;i<lenth;)
    {
        des[i++] = input_buf[(input_r++)&(INPUT_BUF_SIZE-1)];
        if(input_r==input_e) 
        {
            des[i++] = input_buf[(input_r++)&(INPUT_BUF_SIZE-1)];
            input_r=input_e;
            break;
        }
    }
    if(input_e!=input_r) input_e=input_r;
    pop_off();
    return i;
}
