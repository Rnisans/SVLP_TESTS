
#include "socket_1.h"

//static cycle_buf i_fifo;
//static cycle_buf o_fifo;

volatile cycle_buf i_fifo;
volatile cycle_buf o_fifo;

static void port_init(void){
	FlushBuf(&i_fifo);
	FlushBuf(&o_fifo);
}

Socket::Socket():
    ABstractSocket()
{
}

uint32_t Socket::read(void * const buf, const uint32_t len)
{
	unsigned short size = GetLen(&i_fifo);
	if(len < size) size = len;
	unsigned short i;
	for (i = 0; i < size; i++)
	{
		//__disable_irq();
		(static_cast< char* const>(buf))[i] = GetChar(&i_fifo);
		//__enable_irq();
	}
	return size;
}

uint32_t Socket::skip(uint32_t const )
{
    return 0;
}

bool Socket::is_ready_read()
{
	return GetLen(&i_fifo);
}

void Socket::flush()
{
	//while (USART_GetITStatus(USART1, USART_IT_TC) != SET);
}

uint32_t Socket::write(const void *buf, const uint32_t len)
{
	uint32_t i = 0;
	//__disable_irq();
		for (i = 0; i < len; i++) {
			PutChar(&o_fifo, ((uint8_t*) buf)[i]);
		}
		//__enable_irq();
		if(i != len) while(1);
		return i;
}

bool Socket::open()
{
	port_init();
    return true;
}

bool Socket::close()
{
     return true;
}
