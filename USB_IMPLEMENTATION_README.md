# USB Implementation Using STM32 USB-FS-Device Driver

## Overview
This project has been rewritten to use the STM32 USB-FS-Device Driver library instead of a custom USB implementation. The code now properly utilizes the official STM32 USB library for Virtual COM Port (VCP) functionality.

## Changes Made

### 1. Created USB-VCP.h
- New header file that provides the interface for USB Virtual COM Port functionality
- Includes proper STM32 USB library headers
- Defines USB line coding structure and parameters
- Declares USB VCP functions: `USBInit()`, `USB_MANAGEMENT()`, `Write_VCP()`, `Read_VCP()`

### 2. Created USB-VCP.c
- Implementation file for USB Virtual COM Port functionality
- Uses the STM32 USB library functions
- Implements FIFO buffers for data transfer
- Provides proper USB interrupt handling
- Includes USB state management

### 3. Updated main.cpp
- Replaced missing `ClockInit()` and `SysTick_Init()` with available functions from `hw_config.h`
- Updated include paths to use the STM32 USB library
- Fixed the include path for `svlp_writer.h`
- Improved PID controller with proper string handling
- Added proper USB initialization sequence

### 4. Key Features
- **USB Virtual COM Port**: Full VCP implementation using STM32 USB library
- **FIFO Buffers**: Ring buffer implementation for reliable data transfer
- **PID Controller**: Temperature control with proper USB communication
- **Interrupt Handling**: Proper USB interrupt management
- **Line Coding**: Configurable USB communication parameters

## File Structure
```
├── USB-VCP.h              # USB VCP header file
├── USB-VCP.c              # USB VCP implementation
├── main.cpp               # Updated main application
├── STM32_USB-FS-Device_Driver/  # STM32 USB library
│   ├── inc/              # USB library headers
│   ├── src/              # USB library source files
│   ├── usb_desc.c        # USB descriptors
│   ├── usb_endp.c        # USB endpoints
│   ├── usb_istr.c        # USB interrupt handling
│   ├── usb_prop.c        # USB properties
│   └── usb_pwr.c         # USB power management
└── socket/cycle_buf.h    # FIFO buffer implementation
```

## Usage
The application now properly uses the STM32 USB library for:
- USB device initialization
- Virtual COM Port communication
- Data transmission and reception
- USB state management
- Interrupt handling

## Dependencies
- STM32F10x Standard Peripheral Library
- STM32 USB-FS-Device Driver library
- Custom FIFO buffer implementation
- Temperature sensor library
- SVLP communication library

## Compilation
Make sure to include the STM32 USB library source files in your build configuration:
- `STM32_USB-FS-Device_Driver/src/*.c`
- `STM32_USB-FS-Device_Driver/inc/*.h`
- `socket/cycle_buf1.c`

The code is now properly structured to use the official STM32 USB library while maintaining all the original functionality for temperature control and USB communication.