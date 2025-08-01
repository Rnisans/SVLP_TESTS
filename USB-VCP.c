#include "USB-VCP.h"
#include "STM32_USB-FS-Device_Driver/inc/usb_core.h"
#include "STM32_USB-FS-Device_Driver/inc/usb_init.h"
#include "STM32_USB-FS-Device_Driver/inc/usb_mem.h"
#include "STM32_USB-FS-Device_Driver/inc/usb_sil.h"
#include "string.h"

// Global variables
volatile cycle_buf i_fifo;  // Input FIFO buffer
volatile cycle_buf o_fifo;  // Output FIFO buffer

// USB Line Coding
USB_LINE_CODING linecoding = {
    .dwDTERate = 115200,
    .bCharFormat = 0,
    .bParityType = 0,
    .bDataBits = 8
};

// Line coding parameters
uint32_t LC_DATA_RATE = 115200;
uint8_t LC_STOP_BITS = 0;
uint8_t LC_PARITY_TYPE = 0;
uint8_t LC_N_BITS_DATA = 8;

// USB State
__IO uint32_t bDeviceState = UNCONNECTED;
__IO bool fSuspendEnabled = false;

// USB Initialization
void USBInit(void)
{
    // Initialize FIFO buffers
    FlushBuf(&i_fifo);
    FlushBuf(&o_fifo);
    
    // Initialize USB
    USB_Init();
    
    // Enable USB interrupts
    NVIC_EnableIRQ(USB_LP_CAN0_RX0_IRQn);
    NVIC_EnableIRQ(USB_HP_CAN0_TX_IRQn);
}

// USB Management - called in main loop
void USB_MANAGEMENT(void)
{
    // Handle USB interrupts
    if (bDeviceState == CONFIGURED)
    {
        // USB is configured and ready
        // The actual data transfer is handled in the endpoint callbacks
    }
}

// Write data to USB VCP
void Write_VCP(uint8_t* data, uint16_t length)
{
    if (bDeviceState != CONFIGURED) return;
    
    // Add data to output FIFO
    for (uint16_t i = 0; i < length; i++)
    {
        PutChar(&o_fifo, data[i]);
    }
    
    // Trigger transmission if endpoint is ready
    if (GetEPTxStatus(ENDP1) == EP_TX_NAK)
    {
        EP1_IN_Callback();
    }
}

// Read data from USB VCP
void Read_VCP(uint8_t* data, uint32_t* length)
{
    *length = 0;
    
    if (bDeviceState != CONFIGURED) return;
    
    // Read available data from input FIFO
    while (GetLen(&i_fifo) > 0 && *length < 64)
    {
        data[*length] = GetChar(&i_fifo);
        (*length)++;
    }
}

// Simple delay function for USB operations
void Delay_ms_USB(uint32_t ms)
{
    for (volatile uint32_t i = 0; i < ms * 8000; i++)
    {
        __NOP();
    }
}

// USB Interrupt Handlers
void USB_LP_CAN0_RX0_IRQHandler(void)
{
    USB_Istr();
}

void USB_HP_CAN0_TX_IRQHandler(void)
{
    USB_Istr();
}