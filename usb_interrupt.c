#include "STM32_USB-FS-Device_Driver/inc/usb_lib.h"
#include "STM32_USB-FS-Device_Driver/usb_istr.h"

// USB Low Priority Interrupt Handler
void USB_LP_CAN1_RX0_IRQHandler(void) {
    USB_Istr();
}