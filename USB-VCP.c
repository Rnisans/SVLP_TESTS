#include "USB-VCP.h"

#include "STM32_USB-FS-Device_Driver/inc/usb_lib.h"
#include "STM32_USB-FS-Device_Driver/usb_prop.h"
#include "STM32_USB-FS-Device_Driver/usb_istr.h"
#include "STM32_USB-FS-Device_Driver/usb_pwr.h"

#include "hw_config.h"
#include "delay/delay.h"
#include "socket/cycle_buf.h"

#include "stm32f10x.h"

/* External FIFO buffers declared in socket_1.cpp */
extern volatile cycle_buf i_fifo;
extern volatile cycle_buf o_fifo;

/*---------------------------------------------------------------------------*/
void USBInit(void)
{
    /* Configure clocks, GPIO and interrupts used by USB */
    Set_System();
    Set_USBClock();
    USB_Interrupts_Config();

    /* Initialize the USB device as Virtual COM Port */
    Virtual_Com_Port_init();
}

/*---------------------------------------------------------------------------*/
void USB_MANAGEMENT(void)
{
    /* This helper inside hw_config.c takes care of asynchronous IN transfers */
    Handle_USBAsynchXfer();
}

/*---------------------------------------------------------------------------*/
uint32_t Read_VCP(uint8_t *buf, uint32_t *len)
{
    if (!buf || !len || *len == 0) {
        return 0;
    }

    uint32_t available = GetLen(&i_fifo);
    if (available > *len) {
        available = *len;
    }

    for (uint32_t i = 0; i < available; ++i) {
        buf[i] = GetChar(&i_fifo);
    }

    *len = available;
    return available;
}

/*---------------------------------------------------------------------------*/
uint32_t Write_VCP(const uint8_t *buf, uint32_t len)
{
    if (!buf || len == 0) {
        return 0;
    }

    for (uint32_t i = 0; i < len; ++i) {
        PutChar(&o_fifo, buf[i]);
    }
    return len;
}

/*---------------------------------------------------------------------------*/
void Delay_ms_USB(uint32_t ms)
{
    delay_ms(ms);
}