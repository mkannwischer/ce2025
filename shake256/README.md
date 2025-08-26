# SHAKE256 Project

This directory contains a SHAKE256 implementation with dual-platform support for STM32F407 hardware and QEMU emulation.

## Adding Your Own Sources

To add your own C or assembly files, modify the `Makefile`:

1. **C Sources**: Add to `PROJECT_C_SOURCES`
   ```makefile
   PROJECT_C_SOURCES = test.c shake256.c mynewfile.c
   ```

2. **Assembly Sources**: Add to `PROJECT_ASM_SOURCES` 
   ```makefile
   PROJECT_ASM_SOURCES = myfunction.S
   ```

The build system will automatically compile and link your sources.

## Building

- **QEMU**: `make PLATFORM=qemu`
- **STM32F407**: `make PLATFORM=stm32`

## Running

- **QEMU**: `make run-qemu PLATFORM=qemu`
- **STM32F407**: `make flash-stm32 PLATFORM=stm32` (requires st-link tools)

## Output Files

- `bin/qemu.bin` - QEMU binary
- `bin/stm32f407.bin` - STM32F407 binary  
- `elf/qemu.elf` - QEMU ELF (for debugging)
- `elf/stm32f407.elf` - STM32F407 ELF (for debugging)

## Testing

The program runs both correctness tests and performance benchmarks:
- ✅ **Success**: Ends with `*** ALL GOOD ***`
- ❌ **Failure**: Ends with `*** TEST FAILED ***`

Note: Performance benchmarks are disabled in QEMU as cycle counts are not meaningful in emulation.

## Flash Requirements

For STM32F407 flashing, install ST-Link tools:
- Ubuntu/Debian: `sudo apt install stlink-tools`
- macOS: `brew install stlink`

## Serial Output

To view output from the STM32F407 board, connect via serial terminal:

### Using pyserial miniterm:
```bash
# Install pySerial if not already installed
pip install pyserial

# Connect to serial port (adjust port and baud rate as needed)
pyserial-miniterm /dev/ttyUSB0 115200
# Common ports:
# Linux: /dev/ttyUSB0
# macOS: /dev/tty.usbserial-*
```