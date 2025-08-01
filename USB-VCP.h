#ifndef USB_VCP_H
#define USB_VCP_H

#include "stm32f10x.h"
#include "STM32_USB-FS-Device_Driver/inc/usb_lib.h"
#include "STM32_USB-FS-Device_Driver/inc/usb_desc.h"
#include "STM32_USB-FS-Device_Driver/inc/usb_pwr.h"
#include "STM32_USB-FS-Device_Driver/inc/usb_istr.h"
#include "STM32_USB-FS-Device_Driver/inc/usb_endp.h"
#include "STM32_USB-FS-Device_Driver/inc/usb_prop.h"
#include "STM32_USB-FS-Device_Driver/inc/usb_mem.h"
#include "socket/cycle_buf.h"

// USB VCP Configuration
extern volatile cycle_buf i_fifo;  // Input FIFO buffer
extern volatile cycle_buf o_fifo;  // Output FIFO buffer

// Line coding structure for USB CDC
typedef struct {
    uint32_t bitrate;
    uint8_t format;
    uint8_t paritytype;
    uint8_t datatype;
} LINE_CODING;

// Global variables for line coding
extern LINE_CODING linecoding;
extern uint8_t linecoding_updated;

// Function prototypes
void USBInit(void);
void USB_MANAGEMENT(void);
void Write_VCP(uint8_t* data, uint16_t length);
void Read_VCP(uint8_t* data, uint32_t* length);
void Delay_ms_USB(uint32_t ms);

// USB state management
extern volatile uint8_t bDeviceState;
extern volatile uint8_t bDeviceStatus;

// USB endpoint status
#define EP_TX_NAK      0x00000000
#define EP_TX_VALID    0x00000002
#define EP_TX_STALL    0x00000006

#endif // USB_VCP_H