#ifndef __USB_VCP_H
#define __USB_VCP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Initialize USB virtual COM port */
void USBInit(void);

/* Should be called periodically from main loop to handle async USB transfers */
void USB_MANAGEMENT(void);

/* Read data from host.  *len is input (buffer size) and output (bytes actually read) */
uint32_t Read_VCP(uint8_t *buf, uint32_t *len);

/* Write data to host */
uint32_t Write_VCP(const uint8_t *buf, uint32_t len);

/* Millisecond delay that can safely be used while USB is running */
void Delay_ms_USB(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif /* __USB_VCP_H */