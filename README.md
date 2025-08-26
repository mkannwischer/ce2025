# NTU Cryptographic Engineering 2025 - Final Projects

This repository contains implementations of various cryptographic algorithms targeting Arm Cortex-M4 microcontrollers. Code can be tested using QEMU emulation or deployed on a STM32F407 development board for performance benchmarking.

## Projects

### Project Structure

Projects are organized into two parts:

**Part A: Classical Cryptography**
- **shake256/**: SHAKE256 extendable output function (XOF)
- **ecdh25519**: Elliptic Curve Diffie-Hellman key exchange using Curve25519

**Part B: Post-Quantum Cryptography**
- **ml-kem/**: ML-KEM (Kyber) lattice-based key encapsulation mechanism
- **ml-dsa/**: ML-DSA (Dilithium) lattice-based digital signature algorithm  

### Completion Requirements

- **Groups of 2**: Complete all 4 projects (both Part A and Part B)
- **Individual work**: Complete one project from Part A and one project from Part B

For more details on the final project (required tasks, hints, and grading details), refer to the final project document in NTU COOL.

## Quick Start

### Prerequisites

**Arm GCC Toolchain**:
- **Preferred**: Download from [Arm GNU Toolchain Downloads](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)
- Ubuntu/Debian: `sudo apt install gcc-arm-none-eabi`
- macOS: `brew install arm-none-eabi-gcc`

**QEMU with Arm support**:
- Ubuntu/Debian: `sudo apt install qemu-system-arm`
- macOS: `brew install qemu`

**For STM32F407 hardware**:
- Ubuntu/Debian: `sudo apt install stlink-tools`
- macOS: `brew install stlink`

**For serial output**:
- Ubuntu/Debian: `sudo apt install python3-serial`
- Cross-platform: `pip install pyserial`

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
make flash-stm32 PLATFORM=stm32 # Flash to hardware
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
├── testvectors.inc          # NIST test vectors (where applicable)
├── *.c, *.h                 # Implementation files
└── README.md                # Project-specific documentation
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
- **Performance Benchmarks**: Cycle counting (STM32F407 only)

**Test Results**:
- **Success**: Ends with `*** ALL GOOD ***`
- **Failure**: Ends with `*** TEST FAILED ***`

**Note**: Performance benchmarks are disabled in QEMU as cycle counts are not meaningful in emulation.

## Hardware Setup

### STM32F407 Serial Output

To view output from the STM32F407 board, connect via serial terminal:

```bash
# Connect to serial port (adjust port and baud rate as needed)
pyserial-miniterm /dev/ttyUSB0 115200

# Common ports:
# Linux: /dev/ttyUSB0, /dev/ttyACM0
# macOS: /dev/tty.usbserial-*, /dev/tty.usbmodem*
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