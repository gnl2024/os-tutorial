# WISP-BB Quick Reference Guide

## 🚀 Quick Start

```bash
cd 21-shell
make clean && make
make run
```

## 📁 Key Files

### Core System:
- `boot/bootsect.asm` - Boot sector (53 lines)
- `kernel/kernel.c` - Main kernel entry
- `cpu/gdt.c` - Memory management
- `cpu/isr.c` - Interrupt handling
- `drivers/screen.c` - Video driver
- `drivers/keyboard.c` - Keyboard driver
- `libc/mem.c` - Memory allocation

### Build System:
- `Makefile` - Build configuration
- `cpu/types.h` - Type definitions

## 🔧 Key Functions

### Memory Management:
```c
u32 kmalloc(u32 size, int align, u32 *phys_addr);
void memory_copy(u8 *source, u8 *dest, int nbytes);
void memory_set(u8 *dest, u8 val, u32 len);
```

### Video Driver:
```c
void kprint(char *message);
void kprint_at(char *message, int col, int row);
void clear_screen();
```

### String Utilities:
```c
void int_to_ascii(int n, char str[]);
int strlen(char s[]);
int strcmp(char s1[], char s2[]);
void append(char s[], char n);
```

### Interrupt System:
```c
void isr_install();
void irq_install();
void register_interrupt_handler(u8 n, isr_t handler);
```

## 📊 System Statistics

- **Total Assembly**: 171 lines (4 files)
- **Total C Code**: 15 files
- **Assembly Reduction**: 83% (from ~1000+ lines)
- **Memory Model**: Flat 32-bit protected mode
- **Video**: VGA text mode (80x25)
- **Input**: PS/2 keyboard
- **Memory**: Page-aligned allocation

## 🎯 Current Features

### ✅ Working:
- Boot sector with disk loading
- Protected mode transition
- GDT setup and memory management
- Complete interrupt system (ISRs + IRQs)
- VGA video driver with scrolling
- PS/2 keyboard input
- Memory allocation (`kmalloc`)
- String manipulation library
- Error handling

### 🔧 Remaining Assembly:
1. `cpu/gdt_flush.asm` (14 lines) - GDT loading
2. `cpu/isr_stubs_simple.asm` (99 lines) - ISR stubs
3. `boot/bootsect.asm` (53 lines) - Boot sector
4. `boot/kernel_entry.asm` (5 lines) - Kernel entry

## 🚀 Testing Commands

```bash
# Build system
make clean && make

# Run in QEMU
make run

# Debug with GDB
make debug
gdb kernel.elf

# Test features
# 1. Type characters - should appear on screen
# 2. Type "END" - should halt system
# 3. Fill screen - should scroll
```

## 📚 Chapter Status

### ✅ Complete (00-24):
- **00-11**: Boot, protected mode, cross-compiler
- **12-14**: C kernel setup
- **15-17**: Video driver system
- **18-21**: Interrupt system
- **22**: Memory allocator
- **23**: String library enhancements
- **24**: Build system

## 🎯 Next Phase: Microkernel

The system is **complete and stable**. Ready for:
1. Process isolation
2. Inter-process communication (IPC)
3. User space services
4. Hardware abstraction layer (HAL)

---

*Quick Reference - WISP-BB v1.0* 