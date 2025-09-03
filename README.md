# NTU Cryptographic Engineering 2025 - Final Projects

This repository contains implementations of various cryptographic algorithms cross-compiled for an Arm Cortex-M4 microcontrollers. Code can be tested using QEMU emulation or deployed on a STM32F407 development board for performance benchmarking.

## Projects

### Project Structure

Projects are organized into two parts:

**Part A: Classical Cryptography**
- **shake256/**: SHAKE256 extendable output function (XOF)
- **ecdh25519/**: Elliptic Curve Diffie-Hellman key exchange using Curve25519

**Part B: Post-Quantum Cryptography**
- **ml-kem/**: ML-KEM (Kyber) lattice-based key encapsulation mechanism
- **ml-dsa/**: ML-DSA (Dilithium) lattice-based digital signature algorithm  

### Completion Requirements

- **Groups of 2**: Complete all 4 projects (both Part A and Part B)
- **Individual work**: Complete one project from Part A and one project from Part B

For more details on the final project (required tasks, hints, and grading details), refer to the final project document in NTU COOL.

## Quick Start

### Prerequisites

Linux (recommended: Ubuntu) or MacOS is required to complete this project.
If you are using Windows, we highly recommend using an Ubuntu virtual machine, e.g., using [Virtual Box](https://www.virtualbox.org/).

**Arm GCC Toolchain**:
- **Preferred**: Download from [Arm GNU Toolchain Downloads](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)
- Ubuntu/Debian: `sudo apt install gcc-arm-none-eabi`
- macOS: `brew install gcc-arm-embedded`

**QEMU with Arm support**:
- Ubuntu/Debian: `sudo apt install qemu-system-arm`
- macOS: `brew install qemu`

**For STM32F407 hardware**:
- Ubuntu/Debian: `sudo apt install stlink-tools`
- macOS: `brew install stlink`

**For serial output**:
- Ubuntu/Debian: `sudo apt install python3-serial`
- Cross-platform: `pip install pyserial`

### Cloning

Clone the starter package with its submodules
```
git clone --recursive https://github.com/mkannwischer/ce2025
cd ce2025
```

### Building and Running

Navigate to any project directory and use these commands:

**QEMU (Emulation)**:
```bash
cd ml-dsa/                    # or any project directory
make PLATFORM=qemu            # Build
make run-qemu PLATFORM=qemu   # Build and run
```

**STM32F407 (Hardware)**:
```bash
cd ml-dsa/                      # or any project directory  
make PLATFORM=stm32             # Build
make flash-stm32 PLATFORM=stm32 # Build and flash to hardware
```

**Code Size Analysis**:
```bash
cd ml-dsa/                      # or any project directory
make size PLATFORM=qemu         # Analyze code size (works with any platform)
```

### Test All Projects
```bash
./run-all-tests.sh            # Run tests for all projects
```

## Project Structure

Each project follows a consistent structure:
```
project-name/
├── Makefile                 # Project-specific build configuration
├── test.c                   # Main test program with benchmarks
├── *.c, *.h                 # Implementation files
```

## Adding Your Own Sources

To add your own C or assembly files to any project, modify its `Makefile`:

1. **C Sources**: Add to `PROJECT_C_SOURCES`
   ```makefile
   PROJECT_C_SOURCES = test.c implementation.c mynewfile.c
   ```

2. **Assembly Sources**: Add to `PROJECT_ASM_SOURCES` 
   ```makefile
   PROJECT_ASM_SOURCES = optimized_function.S
   ```

The build system will automatically compile and link your sources.

## Output Files

Each build generates platform-specific binaries:
- `bin/qemu.bin` - QEMU binary
- `bin/stm32f407.bin` - STM32F407 binary  
- `elf/qemu.elf` - QEMU ELF (for debugging)
- `elf/stm32f407.elf` - STM32F407 ELF (for debugging)

## Testing

All programs run comprehensive test suites including:
- **Correctness Tests**: Validation against NIST test vectors
- **Functional Tests**: End-to-end algorithm testing
- **Performance Benchmarks**: Cycle count measurements for key operations
- **Stack Usage Analysis**: Memory consumption measurements for each function
- **Code Size Analysis**: Code (binary) size breakdown by source file

**Test Results**:
- **Success**: Ends with `*** ALL GOOD ***`
- **Failure**: Ends with `*** TEST FAILED ***`

**Note**: Performance benchmarks show placeholder messages in QEMU as cycle counts are not meaningful in emulation.

## Hardware Setup

### STM32F407 Serial Output

To view output from the STM32F407 board, connect via serial terminal:

```bash
# Connect to serial port
pyserial-miniterm <PORT> 115200

# Common ports:
# Linux: /dev/ttyUSB0, /dev/ttyACM0
pyserial-miniterm /dev/ttyUSB0 115200

# macOS: /dev/tty.usbserial-*, /dev/tty.usbmodem*
pyserial-miniterm /dev/tty.usbserial-110 115200
```

## Development

### Common Infrastructure
- `common/`: Shared build system, HAL, and utilities
- `common/common.mk`: Common Makefile infrastructure
- `common/qemu.mk`: QEMU-specific build configuration  
- `common/stm32f407.mk`: STM32F407-specific build configuration

### Build System Features
- Dual-platform support (QEMU/STM32F407)
- Clean separation of platform-specific code
- Integrated test running and flashing
