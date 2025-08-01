#include "usb_vcp_wrapper.h"

// Include the STM32 USB FS Device Driver headers
#include "STM32_USB-FS-Device_Driver/inc/usb_lib.h"
#include "STM32_USB-FS-Device_Driver/inc/usb_type.h"
#include "STM32_USB-FS-Device_Driver/usb_desc.h"
#include "STM32_USB-FS-Device_Driver/usb_prop.h"
#include "STM32_USB-FS-Device_Driver/usb_pwr.h"
#include "STM32_USB-FS-Device_Driver/usb_istr.h"

// Include cycle buffer for USB communication
#include "socket/cycle_buf.h"

// Include STM32 hardware configuration
#include "hw_config.h"
#include "stm32f10x.h"

// Global line coding variables
uint32_t LC_DATA_RATE = 115200;
uint8_t LC_STOP_BITS = 0;
uint8_t LC_PARITY_TYPE = 0;
uint8_t LC_N_BITS_DATA = 8;

// External line coding from USB library
extern LINE_CODING linecoding;

// Cycle buffer instances for USB communication
volatile cycle_buf i_fifo;  // Input FIFO (USB to MCU)
volatile cycle_buf o_fifo;  // Output FIFO (MCU to USB)

// Initialize USB VCP
void USBInit(void) {
    // Initialize cycle buffers
    FlushBuf(&i_fifo);
    FlushBuf(&o_fifo);
    
    // Synchronize line coding with USB library
    linecoding.bitrate = LC_DATA_RATE;
    linecoding.format = LC_STOP_BITS;
    linecoding.paritytype = LC_PARITY_TYPE;
    linecoding.datatype = LC_N_BITS_DATA;
    
    // Initialize system for USB
    Set_System();
    
    // Set up USB clock
    Set_USBClock();
    
    // Configure USB interrupts
    USB_Interrupts_Config();
    
    // Initialize USB
    USB_Init();
    
    // Connect USB cable
    USB_Cable_Config(ENABLE);
}

// USB Management function - handles USB operations
void USB_MANAGEMENT(void) {
    // This function can be expanded if needed for USB management
    // Currently, USB operations are handled by interrupts and callbacks
    // defined in the STM32_USB-FS-Device_Driver library
}

// Read data from USB VCP
void Read_VCP(uint8_t* buffer, uint32_t* length) {
    uint32_t count = 0;
    *length = 0;
    
    if (buffer == NULL || length == NULL) {
        return;
    }
    
    // Read available data from input FIFO
    while (GetLen(&i_fifo) > 0 && count < 20) { // Limit to reasonable buffer size
        buffer[count] = GetChar(&i_fifo);
        count++;
    }
    
    // Null-terminate the string
    if (count < 20) {
        buffer[count] = '\0';
    }
    
    *length = count;
}

// Write data to USB VCP
void Write_VCP(uint8_t* buffer, uint32_t length) {
    if (buffer == NULL || length == 0) {
        return;
    }
    
    // Add data to output FIFO
    for (uint32_t i = 0; i < length; i++) {
        PutChar(&o_fifo, buffer[i]);
    }
    
    // The actual USB transmission is handled by the EP1_IN_Callback
    // which is called periodically by the SOF_Callback
}

// Initialize VCP (internal function)
void VCP_Initialize(void) {
    // Initialize the USB device
    Virtual_Com_Port_init();
}

// Set line coding (internal function)
void VCP_SetLineCoding(LINE_CODING_t* line_coding) {
    if (line_coding != NULL) {
        LC_DATA_RATE = line_coding->bitrate;
        LC_STOP_BITS = line_coding->format;
        LC_PARITY_TYPE = line_coding->paritytype;
        LC_N_BITS_DATA = line_coding->datatype;
    }
}

// Get line coding (internal function)
void VCP_GetLineCoding(LINE_CODING_t* line_coding) {
    if (line_coding != NULL) {
        line_coding->bitrate = LC_DATA_RATE;
        line_coding->format = LC_STOP_BITS;
        line_coding->paritytype = LC_PARITY_TYPE;
        line_coding->datatype = LC_N_BITS_DATA;
    }
}