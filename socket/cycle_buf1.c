#include "cycle_buf.h"

#include <stddef.h>
#include "string.h"

//"очищает" буфер
void FlushBuf(volatile cycle_buf* fifo)
{
	fifo->tail = 0;
	fifo->head = 0;
	fifo->count = 0;
	memset((unsigned char *)fifo->cycleBuf,-1,SIZE_BUF);
}

//положить символ в буфер
void PutChar(volatile cycle_buf* fifo, unsigned char sym)
{
	if (fifo->count < SIZE_BUF){   //если в буфере еще есть место
		if(fifo->cycleBuf[fifo->tail] == (unsigned char)-1)
		{
			fifo->cycleBuf[fifo->tail] = sym;    //помещаем в него символ
			fifo->count++;                    //инкрементируем счетчик символов
			fifo->tail++;                           //и индекс хвоста буфера
			if (fifo->tail == SIZE_BUF) fifo->tail = 0;
		}
		else {
			//while(1);
		}
	}
	else
	{
//		while(1);
	}
}

//взять символ из буфера
unsigned char GetChar(volatile cycle_buf* fifo)
{
	unsigned char sym = 0;
	if (fifo->count > 0){                            	//если буфер не пустой
		sym = fifo->cycleBuf[fifo->head];         		//считываем символ из буфера
		fifo->cycleBuf[fifo->head] = -1;
		fifo->count--;                                 //уменьшаем счетчик символов
		fifo->head++;                                  //инкрементируем индекс головы буфера
		if (fifo->head == SIZE_BUF) fifo->head = 0;
	}
	else {
		while(1);
	}
	return sym;
}

//Получит количество символов буфера
unsigned short GetLen(volatile cycle_buf* fifo)
{
	return fifo->count;
}
