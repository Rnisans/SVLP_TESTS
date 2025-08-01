# USB Rewrite Using STM32 USB-FS-Device_Driver Library

## Overview

This project has been rewritten to use the STM32 USB-FS-Device_Driver library instead of a custom USB implementation. The new implementation provides a cleaner interface for USB Virtual COM Port (VCP) functionality.

## Files Created/Modified

### New Files Created:
1. **USB-VCP.h** - Header file for USB Virtual COM Port interface
2. **USB-VCP.c** - Implementation of USB VCP functionality
3. **RCC_CLOCK.h** - Clock configuration header
4. **RCC_CLOCK.c** - Clock initialization implementation
5. **USB_REWRITE_README.md** - This documentation file

### Modified Files:
1. **main.cpp** - Updated to use the new USB-VCP interface
   - Removed Cyrillic comments and replaced with English
   - Updated include paths
   - Simplified initialization code
   - Improved code structure and readability

## Key Changes

### USB Implementation:
- **Before**: Custom USB implementation with missing USB-VCP.h file
- **After**: Uses STM32 USB-FS-Device_Driver library with proper VCP implementation

### Clock Configuration:
- **Before**: Missing RCC_CLOCK.h file
- **After**: Proper clock initialization for 72MHz system clock

### Code Structure:
- **Before**: Cyrillic comments and unclear structure
- **After**: English comments and clean, well-documented code

## Features

### USB Virtual COM Port:
- **Baud Rate**: 115200 (configurable)
- **Data Bits**: 8
- **Stop Bits**: 1
- **Parity**: None
- **Buffer Size**: 512 bytes (configurable in cycle_buf.h)

### PID Temperature Controller:
- **Proportional (Kp)**: 12.0
- **Integral (Ki)**: 0.4
- **Derivative (Kd)**: 4.0
- **Integral Limits**: ±50
- **Output Range**: 0-100%

### Hardware Configuration:
- **System Clock**: 72 MHz
- **USB Clock**: 48 MHz
- **PWM Frequency**: 100 Hz
- **Temperature Sensor**: DS18B20 on GPIOB.11
- **Heater Control**: PWM on GPIOB.0 (TIM3_CH3)

## Build Requirements

### Required Libraries:
1. **STM32F10x Standard Peripheral Library**
2. **STM32 USB-FS-Device_Driver** (included)
3. **OneWire Library** (for DS18B20 temperature sensor)
4. **SVLP Common Library** (for data logging)

### Include Paths:
```
- STM32_USB-FS-Device_Driver/
- stm_lib/
- cmsis/
- cmsis_boot/
- one_wire/
- socket/
- svlp_common_lib/
- delay/
```

## Usage

### Initialization:
```cpp
// Initialize all peripherals
ClockInit();        // Configure 72MHz system clock
USBInit();          // Initialize USB VCP
TIMEInit();         // Configure PWM timer
SysTick_Init();     // Initialize system tick
temperatureInit();   // Initialize DS18B20 sensor
```

### USB Communication:
```cpp
// Send data via USB VCP
char message[20];
snprintf(message, sizeof(message), "%.2f,%d\r\n", temperature, power);
SendMsg(message);

// Read data from USB VCP
uint8_t data[64];
uint32_t length = 64;
Read_VCP(data, &length);
```

### PID Control:
```cpp
// The PID function automatically reads target temperature from USB
// and returns the calculated power output
int power = PID();
```

## API Reference

### USB-VCP Functions:
- `void USBInit(void)` - Initialize USB VCP
- `void USB_MANAGEMENT(void)` - USB state management (call regularly)
- `void Write_VCP(uint8_t* data, uint16_t length)` - Send data
- `void Read_VCP(uint8_t* data, uint32_t* length)` - Receive data
- `void Delay_ms_USB(uint32_t ms)` - USB timing delay

### Clock Functions:
- `void ClockInit(void)` - Initialize 72MHz system clock
- `void SysTick_Init(void)` - Initialize system tick timer

### Temperature Functions:
- `void temperatureInit(void)` - Initialize DS18B20 sensor
- `void updateTemp(void)` - Update temperature reading
- `float getTemp(void)` - Get current temperature

## Troubleshooting

### Common Issues:

1. **USB not recognized**: Check USB cable and ensure proper initialization sequence
2. **Temperature reading errors**: Verify DS18B20 wiring and timing
3. **PID oscillations**: Adjust Kp, Ki, Kd parameters
4. **Build errors**: Ensure all include paths are correctly set

### Debug Features:
- USB VCP provides real-time temperature and power data
- PID parameters can be adjusted via USB commands
- System status is reported every second

## License

This code uses the STM32 USB-FS-Device_Driver library from STMicroelectronics.
All modifications are provided as-is for educational and development purposes.