# UART Bootloader Project for ARM Cortex-M3

Welcome to the UART Bootloader Project for ARM Cortex-M3! This repository contains a versatile UART bootloader that enables efficient code burning, verification, and update for microcontrollers based on the ARM Cortex-M3 architecture.

## Layers

### 1. Library Layer
- `common_macros`: Contains commonly used macros to simplify code readability.
- `std_types`: Provides standardized type definitions for portability.

### 2. MCAL (Microcontroller Abstraction Layer) Layer
- `FPEC`: Flash Programming and Erasing Controller driver for writing and erasing code in flash memory.
- `UART`: UART communication driver for data exchange with the host computer.
- `STK`: SysTick Timer driver for timeout management and timing.
- `NVIC`: Nested Vectored Interrupt Controller driver for handling interrupts.
- `DIO`: Digital Input/Output driver for GPIO management.
- `RCC`: Reset and Clock Control driver for clock configuration.

### 3. APP Layer
- `parse`: Parsing utility for interpreting incoming data and commands.
- `bootloader`: Main bootloader code responsible for receiving, verifying, and burning code into flash memory.

## Bootloader Functionality

The bootloader offers the following key features:

- **Code Reception**: The bootloader can receive new firmware code via UART communication.
- **Code Verification**: Received code is verified to ensure data integrity before burning.
- **Code Burning**: Verified code is burned into the microcontroller's flash memory for execution.
- **Timeout Mechanism**: In the absence of new code, a timeout mechanism is implemented to trigger the application launch after a certain duration.

## Getting Started

To use the UART bootloader in your project, follow these steps:

1. Configure the microcontroller pins and clock settings according to your hardware requirements.
2. Integrate the required drivers from the Library and MCAL layers into your project.
3. Implement the `parse` and `bootloader` modules in your application layer to handle communication and code burning.
4. Customize the timeout duration in the bootloader to match your application's needs.

For detailed instructions and usage examples, refer to the code comments and documentation within the repository.

## Contribution

Contributions to this repository are welcome! If you find any issues, have enhancements, or want to add support for additional features, please feel free to create pull requests.

## License

This repository is open-source and distributed under the [MIT License](LICENSE).

