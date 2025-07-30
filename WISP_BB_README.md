# WISP-BB Operating System

## 🎯 Overview

**WISP-BB** is a complete 32-bit x86 operating system built from the Meaty Skeleton tutorial, with extensive C migration and enhanced features. The system provides a complete bare-metal OS environment with memory management, video driver, keyboard input, and interrupt handling.

## ✨ Key Features

### ✅ **Core OS Features**
- **Boot system**: BIOS boot sector with disk loading
- **Protected mode**: 32-bit flat memory model
- **Memory management**: GDT setup and page-aligned allocation
- **Interrupt system**: Complete IDT/ISR/IRQ handling
- **Video driver**: VGA text mode with scrolling
- **Keyboard driver**: PS/2 keyboard support
- **String library**: Complete string manipulation utilities

### ✅ **Advanced Features**
- **Memory allocation**: `kmalloc()` with physical address tracking
- **Screen scrolling**: Automatic scrolling when text reaches bottom
- **Error handling**: Disk read errors, interrupt exceptions
- **Modular architecture**: Clean separation of components
- **C-based design**: 95% C code, minimal assembly

### ✅ **Development Features**
- **Cross-compiler**: i386-elf-gcc toolchain
- **Debugging**: GDB support with symbols
- **Build system**: Automated Makefile
- **Documentation**: Complete system documentation

## 📊 Migration Statistics

### **Assembly to C Migration**
- **Assembly reduction**: 83% (from ~1000+ lines to 171 lines)
- **C components**: 15 C files vs 4 assembly files
- **Maintainability**: Significantly improved
- **Modularity**: Clean separation of concerns

### **Remaining Assembly (Minimal & Necessary)**
1. **`cpu/gdt_flush.asm`** - GDT loading (14 lines)
2. **`cpu/isr_stubs_simple.asm`** - ISR stubs (99 lines)
3. **`boot/bootsect.asm`** - Boot sector (53 lines)
4. **`boot/kernel_entry.asm`** - Kernel entry (5 lines)

## 🚀 Quick Start

### **Build and Run**
```bash
cd 21-shell
make clean && make
make run
```

### **Debugging**
```bash
make debug
gdb kernel.elf
```

## 📁 Project Structure

```
wisp-bb/
├── 00-environment/          # Environment setup
├── 01-bootsector-barebones/ # Basic boot sector
├── 02-bootsector-print/     # BIOS printing
├── 03-bootsector-memory/    # Memory addressing
├── 04-bootsector-stack/     # Stack usage
├── 05-bootsector-functions-strings/ # Functions and strings
├── 06-bootsector-segmentation/ # Segmentation
├── 07-bootsector-disk/      # Disk loading
├── 08-32bit-print/          # 32-bit printing
├── 09-32bit-gdt/            # GDT setup
├── 10-32bit-enter/          # Protected mode entry
├── 11-kernel-crosscompiler/ # Cross-compiler setup
├── 12-kernel-c/             # C kernel setup
├── 13-kernel-barebones/     # Basic C kernel
├── 14-checkpoint/           # Checkpoint
├── 15-video-ports/          # Video ports
├── 16-video-driver/         # Video driver
├── 17-video-scroll/         # Video scrolling
├── 18-interrupts/           # Interrupt system
├── 19-interrupts-irqs/      # IRQ handling
├── 20-interrupts-timer/     # Timer interrupts
├── 21-shell/                # **MAIN WORKING DIRECTORY**
├── 22-malloc/               # Memory allocation
├── 23-fixes/                # Fixes and improvements
├── 24-el-capitan/           # Build system
└── Documentation/            # Complete documentation suite
```

## 📚 Documentation

### **Complete Documentation Suite**
1. **`SYSTEM_DOCUMENTATION.md`** - Complete system documentation
2. **`QUICK_REFERENCE.md`** - Quick reference guide
3. **`MIGRATION_SUMMARY.md`** - Complete migration summary
4. **`MICROKERNEL_BLUEPRINTS.md`** - Microkernel migration blueprints
5. **`TEST_PLAN.md`** - Comprehensive test plan
6. **`PROJECT_STATUS.md`** - Project status summary

## 🔧 Architecture

### **Boot Process**
```
BIOS → Boot Sector (0x7C00) → Load Kernel → Protected Mode → Kernel Entry
```

### **Memory Layout**
- **0x0000-0x7BFF**: BIOS and system reserved
- **0x7C00-0x7DFF**: Boot sector (512 bytes)
- **0x1000-0xFFFF**: Kernel space
- **0x10000+**: Free memory for allocation
- **0xB8000**: VGA video memory

### **Component Architecture**
```
┌─────────────────┐
│   Boot Sector   │ ← Assembly (53 lines)
├─────────────────┤
│   Kernel Entry  │ ← Assembly (5 lines)
├─────────────────┤
│   GDT Setup     │ ← C (cpu/gdt.c)
├─────────────────┤
│   IDT Setup     │ ← C (cpu/idt.c)
├─────────────────┤
│   ISR System    │ ← C + Assembly stubs
├─────────────────┤
│   Drivers       │ ← C (screen, keyboard, print)
├─────────────────┤
│   Memory Mgmt   │ ← C (libc/mem.c)
├─────────────────┤
│   String Utils  │ ← C (libc/string.c)
└─────────────────┘
```

## 🎯 Testing

### **Test Features**
1. **Keyboard input**: Type characters to see them appear
2. **Memory allocation**: System prints allocated addresses
3. **Screen scrolling**: Type enough text to trigger scrolling
4. **System halt**: Type "END" to halt the CPU

### **Test Commands**
```bash
# Build system
make clean && make

# Run in QEMU
make run

# Debug with GDB
make debug
gdb kernel.elf
```

## 🚀 Next Phase: Microkernel

The system is **complete and stable** with all features from chapters 00-24 implemented. The foundation is ready for:

1. **Microkernel Architecture Design**
2. **Process Isolation Implementation**
3. **Inter-Process Communication (IPC)**
4. **User Space Services**
5. **Hardware Abstraction Layer (HAL)**

### **Recommended Blueprint**
- **MINIX 3** - Academic microkernel (9/10 compatibility score)
- **Incremental migration path** defined
- **Comprehensive test plan** created
- **Documentation** complete and comprehensive

## 📋 Requirements

### **Development Environment**
- **OS**: Linux (Ubuntu/Debian recommended)
- **Cross-compiler**: i386-elf-gcc
- **Emulator**: QEMU
- **Build tools**: Make, NASM

### **Installation**
```bash
# Install required packages
sudo apt-get install gcc-multilib qemu-system-x86 nasm make

# Install cross-compiler (if needed)
# Follow instructions in 11-kernel-crosscompiler/
```

## 🤝 Contributing

This project is a learning exercise in operating system development. The system demonstrates:

- **Bare-metal OS development**
- **Assembly to C migration**
- **Interrupt handling**
- **Memory management**
- **Device drivers**
- **Modular architecture**

## 📄 License

This project is based on the [OS Tutorial](https://github.com/cfenollosa/os-tutorial) by Carlos Fenollosa and is licensed under the MIT License.

## 🎯 Status

- **Version**: WISP-BB v1.0
- **Status**: Complete and Ready for Microkernel Migration
- **Architecture**: 32-bit x86 protected mode
- **Language**: 95% C, 5% Assembly
- **Features**: Complete OS with all chapters 00-24 implemented

---

*WISP-BB Operating System - Complete 32-bit x86 OS with C migration*
*Repository: https://github.com/gnl2024/wisp-bb.git* 