
#include "stddef.h"
#define SIZE_BUF 512
//кольцевой (циклический) буфер
typedef struct {
	unsigned char cycleBuf[SIZE_BUF];
	volatile unsigned short tail ;      //"указатель" хвоста буфера
	volatile unsigned short head ;   //"указатель" головы буфера
	volatile unsigned short count ;  //счетчик символов
} cycle_buf;

void FlushBuf(volatile cycle_buf* fifo);
void PutChar(volatile cycle_buf* fifo, unsigned char sym);

unsigned char GetChar(volatile cycle_buf* fifo);
unsigned short GetLen(volatile cycle_buf* fifo);

//bool isFull(volatile cycle_buf* fifo);

