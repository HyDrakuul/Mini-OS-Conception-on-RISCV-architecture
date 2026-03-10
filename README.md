# Mini-OS Conception on RISC-V Architecture

[![Build Status](https://pcs-louising-a1584e.pages.ensimag.fr/lastupdate.svg)](https://pcs-louising-a1584e.pages.ensimag.fr/pcsef.log)

A minimal operating system designed for RISC-V architecture, implementing core OS features including console management, UART communication, timer interrupts, and basic process scheduling.

## Table of Contents

- [Features](#features)
- [Project Structure](#project-structure)
- [Building](#building)
- [Features Demonstration](#features-demonstration)
  - [Console Display](#console-display)
  - [UART Communication](#uart-communication)
  - [Timer Management](#timer-management)
  - [Process Management](#process-management)
- [Implementation Details](#implementation-details)
- [Testing](#testing)

## Features

### Console Display Management
- Simple text output to console
- Support for special characters: LF (line feed), CR (carriage return), HT (horizontal tab), BS (backspace), FF (form feed)
- Cursor management with automatic scrolling
- Real-time cursor positioning

### UART Communication
- UART configuration and initialization
- Printf-style output over serial interface
- Full duplex communication support

### Timer Interrupts
- Timer initialization and configuration
- Interrupt vector setup
- Real-time tick tracking in seconds
- Trap handler for timer events

### Process Management
- Process creation and initialization
- Basic process scheduler (round-robin)
- Process states: Running, Sleeping, Terminated
- Sleep/Wake functionality for multiple processes
- Automatic process state visualization

## Project Structure

```
.
├── src/
│   ├── start.c           # Main entry point and test cases
│   ├── processus.c       # Process management implementation
│   ├── processus.h       # Process management headers
│   ├── console.c         # Console/display driver
│   ├── console.h         # Console headers
│   ├── timer.c          # Timer and interrupt handling
│   ├── timer.h          # Timer headers
│   └── uart.c           # UART communication driver
├── Makefile             # Build configuration
├── .gitlab-ci.yml       # CI/CD pipeline
└── README.md            # This file
```

## Building

### Prerequisites

You need the RISC-V toolchain installed:

```bash
# On Ubuntu/Debian
sudo apt-get install gcc-riscv64-unknown-elf binutils-riscv64-unknown-elf

# Or using a package manager like Homebrew on macOS
brew install riscv-tools
```

### Compilation

```bash
make              # Build the project
make clean        # Remove build artifacts
make distclean    # Remove all generated files including binaries
make disasm       # Generate disassembly listing
make symbols      # Display symbol table
make help         # Show build help
```

The project produces a RISC-V executable that can be run on a RISC-V emulator (like QEMU) or hardware.

### Running on QEMU

```bash
# Install QEMU for RISC-V
sudo apt-get install qemu-system-riscv32

# Run the compiled binary
qemu-system-riscv32 -machine virt -kernel bin/mini-os.elf -nographic
```

## Features Demonstration

### Console Display

The console driver supports various terminal control sequences for proper text output and cursor management:

![Simple Display](https://pcs-louising-a1584e.pages.ensimag.fr/Affichage_simple.svg)
![Line Feed](https://pcs-louising-a1584e.pages.ensimag.fr/LF_new_line.svg)
![Horizontal Tab](https://pcs-louising-a1584e.pages.ensimag.fr/HT_horizontal_tab.svg)
![Backspace](https://pcs-louising-a1584e.pages.ensimag.fr/BS_backspace.svg)
![Carriage Return](https://pcs-louising-a1584e.pages.ensimag.fr/CR_carriage_ret.svg)
![Simple Cursor](https://pcs-louising-a1584e.pages.ensimag.fr/Curseur_simple.svg)
![Scrolling](https://pcs-louising-a1584e.pages.ensimag.fr/scroll.svg)
![Cursor After Scroll](https://pcs-louising-a1584e.pages.ensimag.fr/Curseur_après_scroll.svg)
![Form Feed](https://pcs-louising-a1584e.pages.ensimag.fr/FF_formfeed.svg)
![Cursor After Form Feed](https://pcs-louising-a1584e.pages.ensimag.fr/Curseur_après_FF.svg)

### UART Communication

Serial communication is configured and managed for low-level I/O operations:

![UART Configuration](https://pcs-louising-a1584e.pages.ensimag.fr/UART_config.svg)
![Printf over UART](https://pcs-louising-a1584e.pages.ensimag.fr/printf_UART.svg)

### Timer Management

Timer interrupts provide real-time tracking and periodic task scheduling:

![Timer Initialization](https://pcs-louising-a1584e.pages.ensimag.fr/init_traitant_timer.svg)
![Timer Enable MIE](https://pcs-louising-a1584e.pages.ensimag.fr/enable_timer_mie.svg)
![Timer Reactivation](https://pcs-louising-a1584e.pages.ensimag.fr/enable_timer_re-active.svg)
![Trap Handler Reactivation](https://pcs-louising-a1584e.pages.ensimag.fr/trap_handler_re-active.svg)
![Time Display](https://pcs-louising-a1584e.pages.ensimag.fr/trap_handler_affichage_du_temps.svg)
![Seconds Counter](https://pcs-louising-a1584e.pages.ensimag.fr/nbr_secondes.svg)

### Process Management

Demonstrates the core OS functionality with process creation, scheduling, and lifecycle management:

![Process Initialization](https://pcs-louising-a1584e.pages.ensimag.fr/init_proc_mon_nom_mon_pid.svg)
![Simple Scheduling](https://pcs-louising-a1584e.pages.ensimag.fr/cree_processus_ordonnance_simple.svg)
![Double Scheduling](https://pcs-louising-a1584e.pages.ensimag.fr/cree_processus_ordonnance_double.svg)
![Scheduling After Reset](https://pcs-louising-a1584e.pages.ensimag.fr/cree_processus_ordonnance_apres_reset.svg)
![Process by Interrupt](https://pcs-louising-a1584e.pages.ensimag.fr/Processus_par_interruption.svg)

#### Sleep and Wake Functions

![Two Sleeping Processes](https://pcs-louising-a1584e.pages.ensimag.fr/dors_2_processus_endormis.svg)
![Wake One Process](https://pcs-louising-a1584e.pages.ensimag.fr/dors_reveil_d'un_processus.svg)
![Wake All Processes](https://pcs-louising-a1584e.pages.ensimag.fr/dors_reveil_de_tous_les_processus.svg)

#### Process Termination

![Explicit Termination](https://pcs-louising-a1584e.pages.ensimag.fr/terminaison_explicite.svg)
![Implicit Termination](https://pcs-louising-a1584e.pages.ensimag.fr/terminaison_implicite.svg)

## Implementation Details

### Code Organization

The project is organized into distinct, modular components:

- **start.c**: Main entry point containing test cases for all features. Test functions can be toggled by commenting/uncommenting specific calls.

- **processus.c/h**: Process management subsystem including:
  - Process creation and initialization
  - Round-robin scheduler implementation
  - Process state management
  - Sleep/wake mechanism for process synchronization

- **console.c/h**: Terminal display driver with support for:
  - Text output to console
  - Special character handling (LF, CR, BS, HT, FF)
  - Cursor management and scrolling

- **timer.c/h**: Timer and interrupt handling including:
  - Timer configuration
  - Interrupt vector setup
  - Trap handler implementation
  - Tick counting and time tracking

- **uart.c**: Serial communication driver for low-level I/O

### Process State Visualization

A `affiche_etat()` function provides real-time process state visualization in the top-left corner of the screen, showing:
- Process name
- Process ID
- Current state (Running, Sleeping, Terminated)
- CPU time used

This can be enabled by uncommenting the function call at line 110 in `processus.c`.

**Note**: This visualization may interfere with timer display tests, so it is disabled by default in the test suite.

### Scheduler

The process scheduler uses a simple round-robin algorithm:
- Each runnable process gets a time slice
- Upon timer interrupt, the next process in queue is selected
- Sleeping processes are skipped
- Terminated processes are removed from scheduling

## Testing

All minimum project requirements have been successfully implemented and tested.

### Running Tests

Test cases are located in `start.c` and can be toggled by commenting/uncommenting test function calls:

```c
// In start.c, uncomment the test you want to run:
// test_console();      // Test console display features
// test_uart_printf();  // Test UART communication
// test_timer();        // Test timer and interrupts
// test_processes();    // Test process management
```

### Test Results

Continuous integration test results and logs can be viewed here:
[CI/CD Test Log](https://pcs-louising-a1584e.pages.ensimag.fr/pcsef.log)

The project passes all minimum requirements with no extensions implemented.

## Requirements

- RISC-V 32-bit toolchain (riscv64-unknown-elf or similar)
- Make build system
- QEMU or RISC-V hardware for execution (optional)
- Linux/Unix environment (or WSL on Windows)

## License

Educational Project - Ensimag

## Contributors

Created as part of the Ensimag curriculum.

## Acknowledgments

This project demonstrates fundamental OS concepts including:
- Bare-metal programming
- Assembly language integration
- Interrupt handling
- Process scheduling
- Memory management basics
- Low-level hardware interaction

For educational purposes at Ensimag engineering school.

