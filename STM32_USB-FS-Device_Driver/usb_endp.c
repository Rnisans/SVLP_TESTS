/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
* File Name          : usb_endp.c
* Author             : MCD Application Team
* Version            : V3.3.0
* Date               : 21-March-2011
* Description        : Endpoint routines
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_mem.h"
#include "usb_istr.h"
#include "usb_pwr.h"

#include "cycle_buf.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

volatile extern cycle_buf i_fifo;
volatile extern cycle_buf o_fifo;

/* Interval between sending IN packets in frame number (1 frame = 1ms) */
/* Сколько фреймов будет между отсылкой наших пакетов. */
#define VCOMPORT_IN_FRAME_INTERVAL             5

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// буфер приема данных из USB
uint8_t USB_Rx_Buffer[VIRTUAL_COM_PORT_DATA_SIZE];
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : EP1_IN_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int count=0;
uint8_t needToSend = 0;
// выполняем отправку данных
void EP1_IN_Callback (void)
{
	/*while (GetEPTxStatus(ENDP1) != EP_TX_NAK){// Wait for sending to finish
	   //if (timeout++ > 10000)//60000 //конечная точка работает, но пока не имеет данных для отправки. Готовы к передаче, но данных для передачи нет
	    	return;
	}*/

	__disable_irq();
		if(GetLen(&o_fifo) && GetEPTxStatus(ENDP1) == EP_TX_NAK){//==EP_TX_NAK, ==EPTX_DTOG2, != EP_TX_VALID
		 	//char ch;
		 	uint8_t ch;
		 	ch=(uint8_t)GetChar(&o_fifo);
	   	   	USB_SIL_Write(EP1_IN, &ch, 1);//&ch
	   	   	//(char *)ch
	   	   	SetEPTxValid(ENDP1);//После успешной транзакции состояние VALID сменится на NAK
		}
		//SetEPTxValid(ENDP1);
	__enable_irq();
	//Handle_USBAsynchXfer(); GetEPTxStatus( ENDP1 ) != EP_TX_DIS /*&& GetEPTxStatus( ENDP1 ) == EP_TX_VALID*/
}
/*******************************************************************************
* Function Name  : EP3_OUT_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
// коллбэк при приеме данных от хоста. Анализируем посимвольно и мигаем светодиодами
void EP3_OUT_Callback(void)
{
  uint16_t USB_Rx_Cnt;
  uint16_t i;

  //Get the received data buffer and update the counter
  USB_Rx_Cnt = USB_SIL_Read(EP3_OUT, USB_Rx_Buffer);

  //printf("Hello world");
  //printf("%s\r\n",USB_Rx_Buffer[0]);
  //printf("%d\r\n",USB_Rx_Buffer[0]);
  //printf("%i\r\n",USB_Rx_Buffer[0]);
  //printf("%o\r\n",USB_Rx_Buffer[0]);
  //printf("%u\r\n",USB_Rx_Buffer[0]);
  //printf("%x\r\n",USB_Rx_Buffer[0]);

  for (i=0; i<USB_Rx_Cnt; i++)
  {
	  PutChar(&i_fifo, USB_Rx_Buffer[i]);
  }

  //Enable the receive of data on EP3
  SetEPRxValid(ENDP3);
}
/*******************************************************************************
* Function Name  : SOF_Callback / INTR_SOFINTR_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
// коллбэк по началу фрейма.
// Через определенное количество фреймов сбрасываем накопившийся буфер.
void SOF_Callback(void)
{
  static uint32_t FrameCount = 0;//0
  
  if(bDeviceState == CONFIGURED)
  {
    if (FrameCount++ == VCOMPORT_IN_FRAME_INTERVAL) //FrameCount++
    {
      /* Reset the frame counter */
      FrameCount = 0;//0
      
      /* Check the data to be sent through IN pipe */
      EP1_IN_Callback();
      //Handle_USBAsynchXfer();
    }
  }  
}
