#ifndef __USB_VCP_WRAPPER_H
#define __USB_VCP_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// Line coding structure for USB CDC
typedef struct {
    uint32_t bitrate;
    uint8_t format;
    uint8_t paritytype;
    uint8_t datatype;
} LINE_CODING_t;

// Global line coding variables (equivalent to the old USB-VCP interface)
extern uint32_t LC_DATA_RATE;
extern uint8_t LC_STOP_BITS;
extern uint8_t LC_PARITY_TYPE;
extern uint8_t LC_N_BITS_DATA;

// Function prototypes matching the old USB-VCP interface
void USBInit(void);
void USB_MANAGEMENT(void);
void Read_VCP(uint8_t* buffer, uint32_t* length);
void Write_VCP(uint8_t* buffer, uint32_t length);

// Internal functions
void VCP_Initialize(void);
void VCP_SetLineCoding(LINE_CODING_t* line_coding);
void VCP_GetLineCoding(LINE_CODING_t* line_coding);

#ifdef __cplusplus
}
#endif

#endif /* __USB_VCP_WRAPPER_H */