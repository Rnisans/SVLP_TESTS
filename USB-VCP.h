#ifndef USB_VCP_H
#define USB_VCP_H

#include "stm32f10x.h"
#include "STM32_USB-FS-Device_Driver/inc/usb_lib.h"
#include "STM32_USB-FS-Device_Driver/usb_desc.h"
#include "STM32_USB-FS-Device_Driver/usb_pwr.h"
#include "STM32_USB-FS-Device_Driver/usb_istr.h"
#include "socket/cycle_buf.h"

// USB VCP Configuration
extern volatile cycle_buf i_fifo;  // Input FIFO buffer
extern volatile cycle_buf o_fifo;  // Output FIFO buffer

// USB VCP Functions
void USBInit(void);
void USB_MANAGEMENT(void);
void Write_VCP(uint8_t* data, uint16_t length);
void Read_VCP(uint8_t* data, uint32_t* length);

// USB State Management
extern __IO uint32_t bDeviceState;
extern __IO bool fSuspendEnabled;

// USB Line Coding Structure
typedef struct {
    uint32_t dwDTERate;    // Data terminal rate
    uint8_t bCharFormat;   // Stop bits
    uint8_t bParityType;   // Parity
    uint8_t bDataBits;     // Data bits
} USB_LINE_CODING;

extern USB_LINE_CODING linecoding;

// Line coding parameters
extern uint32_t LC_DATA_RATE;
extern uint8_t LC_STOP_BITS;
extern uint8_t LC_PARITY_TYPE;
extern uint8_t LC_N_BITS_DATA;

// Delay function for USB operations
void Delay_ms_USB(uint32_t ms);

#endif // USB_VCP_H