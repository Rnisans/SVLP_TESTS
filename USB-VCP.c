#include "USB-VCP.h"
#include "hw_config.h"
#include "string.h"
#include "STM32_USB-FS-Device_Driver/inc/usb_lib.h"
#include "STM32_USB-FS-Device_Driver/inc/usb_pwr.h"

// Global variables
volatile cycle_buf i_fifo;  // Input FIFO buffer
volatile cycle_buf o_fifo;  // Output FIFO buffer
LINE_CODING linecoding;
uint8_t linecoding_updated = 0;

// USB state variables
volatile uint8_t bDeviceState = UNCONNECTED;
volatile uint8_t bDeviceStatus = 0;

// Line coding defaults
#define LC_DATA_RATE    115200
#define LC_STOP_BITS    0
#define LC_PARITY_TYPE  0
#define LC_N_BITS_DATA  8

// USB initialization
void USBInit(void) {
    // Initialize FIFO buffers
    FlushBuf(&i_fifo);
    FlushBuf(&o_fifo);
    
    // Initialize line coding
    linecoding.bitrate = LC_DATA_RATE;
    linecoding.format = LC_STOP_BITS;
    linecoding.paritytype = LC_PARITY_TYPE;
    linecoding.datatype = LC_N_BITS_DATA;
    
    // Initialize USB hardware using existing hw_config functions
    Set_System();
    Set_USBClock();
    USB_Interrupts_Config();
    USB_Init();
    
    // Initialize USB device
    Virtual_Com_Port_init();
}

// USB management function - should be called regularly
void USB_MANAGEMENT(void) {
    // Handle USB interrupts and state management
    if (bDeviceState == CONFIGURED) {
        // USB is configured and ready
        // The actual data transfer is handled in the endpoint callbacks
    }
}

// Write data to USB VCP
void Write_VCP(uint8_t* data, uint16_t length) {
    if (data == NULL || length == 0) return;
    
    // Add data to output FIFO
    for (uint16_t i = 0; i < length; i++) {
        PutChar(&o_fifo, data[i]);
    }
    
    // Trigger transmission if USB is ready
    if (bDeviceState == CONFIGURED) {
        // The actual transmission will be handled by EP1_IN_Callback
        // which is called periodically by SOF_Callback
    }
}

// Read data from USB VCP
void Read_VCP(uint8_t* data, uint32_t* length) {
    if (data == NULL || length == NULL) return;
    
    uint32_t available = GetLen(&i_fifo);
    uint32_t to_read = (*length < available) ? *length : available;
    
    for (uint32_t i = 0; i < to_read; i++) {
        data[i] = GetChar(&i_fifo);
    }
    
    *length = to_read;
}

// Delay function for USB timing
void Delay_ms_USB(uint32_t ms) {
    // Simple delay implementation
    // In a real application, you might want to use SysTick or a timer
    for (volatile uint32_t i = 0; i < ms * 72000; i++) {
        __NOP();
    }
}

// USB interrupt handlers (these are called from the USB library)
void USB_LP_CAN0_RX0_IRQHandler(void) {
    USB_Istr();
}

void USB_HP_CAN0_TX_IRQHandler(void) {
    USB_Istr();
}

// Additional USB management functions
void USB_Config(void) {
    // Configure USB endpoints and descriptors
    Virtual_Com_Port_Reset();
}

void USB_Deconfig(void) {
    // Deconfigure USB when device is disconnected
    bDeviceState = UNCONNECTED;
}

// Line coding update function
void Update_LineCoding(void) {
    if (linecoding_updated) {
        // Update line coding parameters
        // This is called when the host changes the line coding
        linecoding_updated = 0;
    }
}